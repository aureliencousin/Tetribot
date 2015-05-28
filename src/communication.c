#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <unistd.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include "communication.h"
#include "data.h"

/*--------------------------------------------------------------------------------------------------------*/
/*fonctions*/

/* processus de communication */
void * reception_thread(void *data);

/* etablit la connexion a l'adresse choisie */
int connection_serveur(const char * hostname, const char * port);

/* formate le message saisi en parametre et l'envoie au serveur */
void envoi_message (char * commande);

/* analyse le type de message pour le renvoyer vers la fonction qui s'occuper de le traiter*/
void analyse_message(char * buff);

/* enregistre le numero de joueur en debut de partie */
void gere_playernum(char * buff);

/* traite les messages recus en debut de partie */
void gere_newgame(char * buff);

/* ajoute au terrain les lignes recues */
void gere_addline(char * buff);

/*--------------------------------------------------------------------------------------------------------*/
/* variables*/
int sock;
char * last_message;
/* variables externes*/
extern char field[264];
extern int isUpdate;
extern int playernum;
extern int fini;
extern int doitPlacer;
extern int block_frequencies[100];
extern int specials_frequencies[100];
extern int lvl;
extern int linePerLevel;
extern int lvlinc;
extern int hfp[12];
/*--------------------------------------------------------------------------------------------------------*/

/* enregistre le numero de joueur en debut de partie */
void gere_playernum(char * buff)
{
    char ** pointeur_msg=&buff;
    strsep(pointeur_msg," ");
    playernum = atoi(*pointeur_msg);
}

/* traite les messages recus en debut de partie */
void gere_newgame(char * buff)
{
    int i;
    char stTemp [2];
    char ** pointeur_msg=&buff;
    char tmp;
    char * block_freq_string = malloc(100*sizeof(char));
    char * specials_freq_string = malloc(100*sizeof(char));
    int j=0;

    strsep(pointeur_msg," ");
    strsep(pointeur_msg," ");

    /* recuperation du niveau et du nombre de lignes par niveau */
	tmp=*pointeur_msg[0];
    lvl=atoi(&tmp);
    strsep(pointeur_msg," ");
    tmp=*pointeur_msg[0];
    linePerLevel=atoi(&tmp);
    strsep(pointeur_msg," ");
   	tmp=*pointeur_msg[0];
    lvlinc=atoi(&tmp);

	for (j=0;j<4;++j){
	    strsep(pointeur_msg," ");
	}

    /* recuperation du message a partir du debut du champ qui indique les frequences de pieces */
    block_freq_string = *pointeur_msg;
    i = 0;
    while (block_freq_string[i] != ' ')
    {
        /* on incremente la frequence d'apparition correspondant a la piece courante */
        sprintf(stTemp,"%c",block_freq_string[i]);
        block_frequencies[i] = atoi(stTemp);
        i++;
    }
    strsep(pointeur_msg," ");
    i=0;
    specials_freq_string = *pointeur_msg;
    /* meme chose pour les cases speciales */
    while (specials_freq_string[i] != ' ')
    {
        /* on incremente la frequence d'apparition correspondant a la piece courante */
        sprintf(stTemp,"%c",specials_freq_string[i]);
        specials_frequencies[i] = atoi(stTemp);
        i++;
    }
}

/* ajoute au terrain les lignes recues */
void gere_addline(char * buff)
{
    char * nbadd;
    int nbaddline;
    int i,j;
    char * pointeur_msg = strstr(buff,"sb 0 cs")+7;
    nbaddline = atoi(pointeur_msg);
    /* pour chaque ligne a ajouter */
    for (i=0; i<nbaddline;i++)
    {
        srand(time(NULL));
        /* on verifie d'abord si le joueur a perdu */
        for(j=0;j<12;j++)
        {
            if(field[j] != '0')
            {
                fini=1;
            }
        }
        /* sinon, on monte le terrain d'une ligne */
        if (fini != 1)
        {
            for(j=12;j<252;j++)
            {
                field[j-12] = field[j];
            }
        }
        /* genere des cellules aleatoires sur la ligne du bas*/
        for (j = 252; j<264; j++)
        {
            field[j] = (rand()%5) + 1 +48;

        }
        /*cree un blanc dans la ligne pour etre conforme au protocole*/
        field[252+rand()%12] = '0';

    }
}

void gere_pause(char * buff)
{
    char ** pointeur_msg=&buff;
    strsep(pointeur_msg," ");
    if (*pointeur_msg[0] == '1')
    {
        doitPlacer = 2;
    }
    else if (*pointeur_msg[0] == '0')
    {
        doitPlacer = 1;
    }
}

void gere_playerwon(char * buff)
{
    char ** pointeur_msg=&buff;
    char * message = malloc(300);
    strsep(pointeur_msg," ");
    if (*pointeur_msg[0] == playernum)
    {
        sprintf(message,"pline %d Essayez encore ;)",playernum);
        envoi_message(message);
    }
    else
    {
        sprintf(message,"pline %d Bien joué...",playernum);
        envoi_message(message);
    }
}

/* traite les messages envoyés par le serveur */
void analyse_message(char * buff)
{
    char * tmp;
    /*Si une partie vient de commencer*/
    if (strncmp("newgame",buff,7) == 0)
    {
        gere_newgame(buff);
    }
    /* si on recoit le numero de joueur */
    if (strncmp("playernum",buff,9) == 0)
    {
        gere_playernum(buff);
    }
    /* si on doit ajouter des lignes */
    if (strstr(buff,"sb 0 cs") != NULL)
    {
        gere_addline(buff);
    }
    /* si quelqu'un met en pause/reprend la partie */
    if(strncmp("pause",buff,5) ==0)
    {
        gere_pause(buff);
    }
    if (strstr(buff,"playerwon") != NULL)
    {
        gere_playerwon(buff);
    }
}

/* etablit la connexion a l'adresse choisie */
int connection_serveur(const char * hostname, const char * port){
    struct addrinfo hints;
    struct addrinfo *res;
    int ret;

    /* On crée une structure avec les informations de connexion */
    memset(&hints, 0, sizeof(struct addrinfo));
    hints.ai_family = AF_INET; /* IPv4 seulement */
    hints.ai_socktype = SOCK_STREAM; /* Socket TCP */
    hints.ai_flags = AI_PASSIVE;

    /* on récupère les informations de l’hôte auquel on veut se connecter */
    if ((ret = getaddrinfo(hostname, port, &hints, &res)) != 0)   {
        fprintf(stderr, "getaddrinfo() : %s\n", gai_strerror(ret));
        exit(EXIT_FAILURE);
    }

    sock = socket(res->ai_family, res->ai_socktype, res->ai_protocol);
    if (sock == -1){
        perror("socket()");
        exit(EXIT_FAILURE);
    }

    if (connect(sock, res->ai_addr, res->ai_addrlen) == -1){
        perror("connect()");
        exit(EXIT_FAILURE);
    }
    return 0;
}

/* formate le message saisi en parametre et l'envoie au serveur */
void envoi_message (char * commande){
    unsigned char * message = malloc(strlen(commande)+1);
    message = (unsigned char * ) strcpy((char *)message,commande);
    message[strlen(commande)] = 0xFF;
    send(sock, message, strlen(commande)+1, 0) ;
}

/* Processus de communication */
void * reception_thread(void *data){
    char * buff = malloc(512);
    int s;
    /*On affiche et on traite les messages reçus en boucle */
    while (1){
        if((s = recv(sock, buff, 512, 0)) < 0){perror("recv()");}
        buff[s] = '\0';
        printf("%s\n",buff);
        analyse_message(buff);
    }
    return NULL;
}



