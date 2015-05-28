

/* initialise la nouvelle piece courante */
int genere_piece();

/* calcule le plus haut point fixe de chaque colonne et remplit la structure correspondante */
void calcHfp();

/* place la piece courante dans le terrain */
char *place();

/* Strucure modelisant la prochaine action a placer sur le terrain */
typedef struct
{
    int sens;
    int posX;
    int posY;
    int piece;
} deplacement ;

/*extern deplacement plcmt;*/
deplacement plcmt;
int doitPlacer;
int doitUpdate;
int hfp[12];
char field[264];
int piece_courante;
