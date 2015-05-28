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
#include "ia.h"
#include "data.h"

/*--------------------------------------------------------------------------------------------------------*/
/*fonctions*/

/* initialise la nouvelle piece courante */
int genere_piece();

/* calcule le plus haut point fixe de chaque colonne et remplit la structure correspondante */
void calcHfp();

/* place la piece courante dans le terrain */
char *place();

/* supprime les lignes pleines du terrain et envoie la demande d'ajout de lignes aux adversaires selon le nombre de lignes remplies */
char * supprime_lignes(char * field);

/*--------------------------------------------------------------------------------------------------------*/
/* variables externes*/
extern int piece_courante;
extern int hfp[12];
extern char field[264];
extern deplacement plcmt;
extern int doitPlacer;
extern int isUpdate;
extern int playernum;
extern int block_frequencies[100];
extern int nbligne;
extern int fini;
extern int lvl;
extern int linePerLevel;
extern int lvlinc;
/*--------------------------------------------------------------------------------------------------------*/

int genere_piece(){
    piece_courante = block_frequencies[rand()%100];
    return piece_courante;
}

void calcHfp(){
	int i,j,end;
	for(i=0;i<12;++i){hfp[i]=-1;}

	for(i=0;i<22;++i){
		for(j=0;j<12;++j){
			if(hfp[(12*i+j)%12]==-1 && field[12*i+j]!='0' ){
				 hfp[(12*i+j)%12]=22-i;
			}
		}
	}

	for(i=0;i<12;++i){
		if(hfp[i]==-1){hfp[i]++;}
	}
}

char * supprime_lignes(char * field) {
    int y,x,col;
    int asupprimer[22];
    int lignepleine = 1;
    char tmpfield[264];
    int ptrfield = 263;
    int nbsup=0;
    char *message=malloc(20);
    /*initialisation du tableau*/
    for(x=0;x<264;x++)
    {
        tmpfield[x] = '0';
    }
    for (y=0;y<22;y++) {
        asupprimer[y] = 0;
    }
    /*parcours du terrain par ligne de bas en haut et note les lignes a supprimer*/
    for(y = 21;y>=0;--y) {
        lignepleine = 1;
        for(x = 0; x < 12 ; ++x) {
            if (field[12*y+x] == '0') {
                lignepleine = 0;
            }
            else if( (12*y+x)<13){fini=1;}
        }
        if (lignepleine != 0) {
            asupprimer[y] = 1;
            nbligne++;
            nbsup++;
            if (nbligne%linePerLevel==0) {
              lvl+=lvlinc;
              printf("félicitation ! pikachu a atteint le niveau %d !\n",lvl);
            }
        }
    }
	/* envoi de la demande d'ajout de lignes aux adversaires */
    if (nbsup>1){
		if (nbsup==2){
			sprintf(message,"sb 0 cs1 %d",playernum);
		}
		if (nbsup==3){
			sprintf(message,"sb 0 cs2 %d",playernum);
		}
		if (nbsup==4){
			sprintf(message,"sb 0 cs4 %d",playernum);
		}
		envoi_message(message);
	}

    /*creation du tableau sans les lignes supprimées*/
    x=263;
    while (x >= 0)
    {
        /*si on ne doit pas supprimer la ligne, on la recopie*/
        if(asupprimer[x/12] == 0)
        {
            for(col=0;col < 12; col++)
            {
                tmpfield[ptrfield] = field[x]  ;
                ptrfield--;
                x--;
            }
        }
        else {
            /* on passe a la ligne suivante */
            x=x-12;
        }

    }
    strcpy(field,tmpfield);

    return field;


}


char *place(){
	switch(plcmt.piece){
/*
	 X X X X
*/
	case 1:	if(plcmt.sens==1){
			field[ 12*(21-(plcmt.posY)) + (plcmt.posX) ] = '1';
			field[ 12*(21-(plcmt.posY)) + (plcmt.posX+1) ] = '1';
			field[ 12*(21-(plcmt.posY)) + (plcmt.posX+2) ] = '1';
			field[ 12*(21-(plcmt.posY)) + (plcmt.posX+3) ] = '1';}
		if(plcmt.sens>=2){
			field[ 12*(21-(plcmt.posY)) + (plcmt.posX) ] = '1';
			field[ 12*(21-(plcmt.posY+1)) + (plcmt.posX) ] = '1';
			field[ 12*(21-(plcmt.posY+2)) + (plcmt.posX) ] = '1';
			field[ 12*(21-(plcmt.posY+3)) + (plcmt.posX) ] = '1';}break;
/*
	X X
    X X
*/
	case 2:	if(plcmt.sens>=1){
			field[ 12*(21-(plcmt.posY)) + (plcmt.posX) ] = '2';
			field[ 12*(21-(plcmt.posY+1)) + (plcmt.posX) ] = '2';
			field[ 12*(21-(plcmt.posY)) + (plcmt.posX+1) ] = '2';
			field[ 12*(21-(plcmt.posY+1)) + (plcmt.posX+1) ] = '2';}break;
/*
    X
    X X X
*/
	case 3:	if(plcmt.sens==1){
			field[ 12*(21-(plcmt.posY)) + (plcmt.posX) ] = '3';
			field[ 12*(21-(plcmt.posY)) + (plcmt.posX+1) ] = '3';
			field[ 12*(21-(plcmt.posY+1)) + (plcmt.posX+1) ] = '3';
			field[ 12*(21-(plcmt.posY+2)) + (plcmt.posX+1) ] = '3';}
		if(plcmt.sens==2){
			field[ 12*(21-(plcmt.posY)) + (plcmt.posX) ] = '3';
			field[ 12*(21-(plcmt.posY+1)) + (plcmt.posX) ] = '3';
			field[ 12*(21-(plcmt.posY)) + (plcmt.posX+1) ] = '3';
			field[ 12*(21-(plcmt.posY)) + (plcmt.posX+2) ] = '3';}
		if(plcmt.sens==3){
			field[ 12*(21-(plcmt.posY)) + (plcmt.posX) ] = '3';
			field[ 12*(21-(plcmt.posY+1)) + (plcmt.posX) ] = '3';
			field[ 12*(21-(plcmt.posY+2)) + (plcmt.posX) ] = '3';
			field[ 12*(21-(plcmt.posY+2)) + (plcmt.posX+1) ] = '3';}
		if(plcmt.sens==4){
			field[ 12*(21-(plcmt.posY+1)) + (plcmt.posX) ] = '3';
			field[ 12*(21-(plcmt.posY+1)) + (plcmt.posX+1) ] = '3';
			field[ 12*(21-(plcmt.posY+1)) + (plcmt.posX+2) ] = '3';
			field[ 12*(21-(plcmt.posY)) + (plcmt.posX+2) ] = '3';}break;
/*
        X
    X X X
*/
	case 4:	if(plcmt.sens==1){
			field[ 12*(21-(plcmt.posY)) + (plcmt.posX) ] ='4' ;
			field[ 12*(21-(plcmt.posY+1)) + (plcmt.posX) ] ='4';
			field[ 12*(21-(plcmt.posY+2)) + (plcmt.posX) ] ='4' ;
			field[ 12*(21-(plcmt.posY)) + (plcmt.posX+1) ] ='4' ;}
		if(plcmt.sens==2){
			field[ 12*(21-(plcmt.posY)) + (plcmt.posX) ] ='4' ;
			field[ 12*(21-(plcmt.posY)) + (plcmt.posX+1) ] ='4' ;
			field[ 12*(21-(plcmt.posY)) + (plcmt.posX+2) ] ='4' ;
			field[ 12*(21-(plcmt.posY+1)) + (plcmt.posX+2) ] ='4' ;}
		if(plcmt.sens==3){
			field[ 12*(21-(plcmt.posY+2)) + (plcmt.posX) ] ='4' ;
			field[ 12*(21-(plcmt.posY+2)) + (plcmt.posX+1) ] ='4' ;
			field[ 12*(21-(plcmt.posY+1)) + (plcmt.posX+1) ] ='4' ;
			field[ 12*(21-(plcmt.posY)) + (plcmt.posX+1) ] ='4' ;}
		if(plcmt.sens==4){
			field[ 12*(21-(plcmt.posY)) + (plcmt.posX) ] ='4' ;
			field[ 12*(21-(plcmt.posY+1)) + (plcmt.posX) ] ='4' ;
			field[ 12*(21-(plcmt.posY+1)) + (plcmt.posX+1) ] ='4' ;
			field[ 12*(21-(plcmt.posY+1)) + (plcmt.posX+2) ] ='4' ;}break;
/*
      X X
    X X
*/
	case 5:	if(plcmt.sens==1){
			field[ 12*(21-(plcmt.posY)) + (plcmt.posX) ] ='1' ;
			field[ 12*(21-(plcmt.posY)) + (plcmt.posX+1) ] ='1' ;
			field[ 12*(21-(plcmt.posY+1)) + (plcmt.posX+1) ] ='1' ;
			field[ 12*(21-(plcmt.posY+1)) + (plcmt.posX+2) ] ='1' ;}
		if(plcmt.sens>=2){
			field[ 12*(21-(plcmt.posY)) + (plcmt.posX+1) ] ='1' ;
			field[ 12*(21-(plcmt.posY+1)) + (plcmt.posX+1) ] ='1' ;
			field[ 12*(21-(plcmt.posY+1)) + (plcmt.posX) ] ='1' ;
			field[ 12*(21-(plcmt.posY+2)) + (plcmt.posX) ] ='1' ;}break;

/*
    X X
      X X
*/
	case 6:	if(plcmt.sens==1){
			field[ 12*(21-(plcmt.posY)) + (plcmt.posX+1) ] ='5' ;
			field[ 12*(21-(plcmt.posY)) + (plcmt.posX+2) ] ='5' ;
			field[ 12*(21-(plcmt.posY+1)) + (plcmt.posX+1) ] ='5' ;
			field[ 12*(21-(plcmt.posY+1)) + (plcmt.posX) ] ='5' ;}
		if(plcmt.sens>=2){
			field[ 12*(21-(plcmt.posY)) + (plcmt.posX) ] ='5' ;
			field[ 12*(21-(plcmt.posY+1)) + (plcmt.posX) ] ='5' ;
			field[ 12*(21-(plcmt.posY+1)) + (plcmt.posX+1) ] ='5' ;
			field[ 12*(21-(plcmt.posY+2)) + (plcmt.posX+1) ] ='5' ;}break;
/*
      X
    X X X
*/
	case 7:	if(plcmt.sens==1){
			field[ 12*(21-(plcmt.posY)) + (plcmt.posX) ] ='2' ;
			field[ 12*(21-(plcmt.posY)) + (plcmt.posX+1) ] ='2' ;
			field[ 12*(21-(plcmt.posY)) + (plcmt.posX+2) ] ='2' ;
			field[ 12*(21-(plcmt.posY+1)) + (plcmt.posX+1) ] ='2' ;}
		if(plcmt.sens==2){
			field[ 12*(21-(plcmt.posY+1)) + (plcmt.posX) ] ='2' ;
			field[ 12*(21-(plcmt.posY+1)) + (plcmt.posX+1) ] ='2' ;
			field[ 12*(21-(plcmt.posY+1)) + (plcmt.posX+2) ] ='2' ;
			field[ 12*(21-(plcmt.posY)) + (plcmt.posX+1) ] ='2' ;}
		if(plcmt.sens==3){
			field[ 12*(21-(plcmt.posY)) + (plcmt.posX) ] ='2' ;
			field[ 12*(21-(plcmt.posY+1)) + (plcmt.posX) ] ='2' ;
			field[ 12*(21-(plcmt.posY+2)) + (plcmt.posX) ] ='2' ;
			field[ 12*(21-(plcmt.posY+1)) + (plcmt.posX+1) ] ='2' ;}
		if(plcmt.sens==4){
			field[ 12*(21-(plcmt.posY)) + (plcmt.posX+1) ] ='2' ;
			field[ 12*(21-(plcmt.posY+1)) + (plcmt.posX+1) ] ='2' ;
			field[ 12*(21-(plcmt.posY+2)) + (plcmt.posX+1) ] ='2' ;
			field[ 12*(21-(plcmt.posY+1)) + (plcmt.posX) ] ='2' ;}
	}
    return field;
}


int main (int argc, char **argv){
    int i;
    int args;
    pthread_t thread_reception;
    pthread_t thread_ia;
    char * message=malloc(300) ;
	if(argc < 3)
	{
		printf("utilisation: ./robot <nom_du_bot> <adresse_du_serveur>\nEx: ./robot pikachu localhost");
		return 1;
	}
	else
	{
		if(connection_serveur(argv[2],"31457") != 0)
		{
			perror("Impossible de se connecter au serveur");
			return 1;
		}
		sprintf(message,"tetrisstart %s 1.13",argv[1]);
		envoi_message(message);
		srand(time(NULL));

		/* on creee le thread qui s'occupe de recevoir les messages envoyés par le serveur ainsi que le thread IA*/
		if (pthread_create(&thread_reception, NULL, reception_thread, (void*)&args) != 0){perror("reception_thread_create()");}
		if (pthread_create(&thread_ia, NULL, ia_thread, (void*)&args) != 0){perror("ia_thread_create()");}

		for(i=0;i<264;++i){field[i]='0';}


		genere_piece();
		calcHfp();
		doitPlacer=0;
		nbligne=0;
		fini=0;
		while(fini==0) {
			if(lvl < 100) {usleep(1005000-(lvl * 10000));}
			else usleep(5000);
			if(piece_courante==0){genere_piece();}
			if(doitPlacer==0){
				place();
				genere_piece();
				sprintf(message,"f %d %s",playernum,supprime_lignes(field));
				envoi_message(message);
				calcHfp();
				doitPlacer=1;
			}


		}
		return 0;
	}

}

