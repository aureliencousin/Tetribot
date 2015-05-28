#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include "ia.h"
#include "data.h"

/*--------------------------------------------------------------------------------------------------------*/
/*fonctions*/

int prepare_placement(deplacement depla);
/* indique au programme principal qu'il a une piece à poser */

deplacement calcule_mouvement(char * field);
/* l'IA calcule le coup à jouer */

/*--------------------------------------------------------------------------------------------------------*/
/*variables*/
int min,i;
/* variables externes*/
extern deplacement plcmt;
extern int doitPlacer;
extern char *field;
extern int piece_courante;
extern int hfp[12];



/*--------------------------------------------------------------------------------------------------------*/

int maxi(int a,int b){
	if(a>b)return a;
	else return b;
}


deplacement calcule_mouvement(char * field){

	deplacement placemt;
	placemt.piece = piece_courante;
	placemt.posY=-1;
	if(doitPlacer==1){
		/*
			 X X X X
		*/
		min=-1;
		if(piece_courante==1){
						
			/* si on a 4 colonnes de suite au meme niveau, placement horizontal */
			for (i=0;i<8;++i){
				if(hfp[i]==hfp[i+1] && hfp[i+1]==hfp[i+2] && hfp[i+2]==hfp[i+3] && hfp[i+3]==hfp[i+4]){
					if(min==-1){min=i;}
					else if (hfp[i]<hfp[min]){min=i;}
				}
			}
			if(min!=-1){
				placemt.posX = min;
				placemt.posY = hfp[min];
	 		    placemt.sens = 1;
				return placemt;
			}
			
			/* sinon, placement vertical a l'endroit le plus bas */
			min=0;
			for (i=1;i<12;++i){
				if (hfp[i]<hfp[min]){min=i;}
			}
			placemt.posX = min;
			placemt.posY = hfp[min];
	 		placemt.sens = 2;
			return placemt;

		 /*
			 X X
			 X X
		*/
		}else if(piece_courante == 2){
		    /* on cherche les deux colonnes colonnes consecutives les plus basses et on y place le carré si elles existent*/
			for(i = 0; i < 11;++i) {
                if(hfp[i] == hfp[i+1]) {
                    if(min == -1) {min=i;}
                    else if (hfp[i]<hfp[min]){min=i;}
                }
			}
			if(min!=-1) {
                    placemt.posX = min;
                    placemt.posY = hfp[min];
                    placemt.sens = 1;
                    return placemt;
            }
            /*sinon, on place le carré le plus bas possible*/
            /* on initialise le maximum si ce n'est pas deja fait */
            if(hfp[0]<=hfp[1]) {min=1;}
            else min=0;
            for(i = 0; i < 11;++i) {
                /* on regarde si le max de deux colonnes consecutives est plus bas que le min */
                if((hfp[i] >= hfp[i+1] && hfp[i+1] < hfp[min]) || (hfp[i] <= hfp[i+1] && hfp[i] < hfp[min])){
                    min=i;
                }
            }
            placemt.posX = min;
            placemt.posY = maxi(hfp[min],hfp[min+1]);
            placemt.sens = 1;
            return placemt;
            
            /* pour la suite des pieces, le bot va essayer de placer la piece le plus bas possible indépendament du sens, sans faire de trou, si c'est impossible il placera avec un trou */           
            
	/*
	X
	X X X
	*/
		}else if (piece_courante==3){
		/*
		X X
		X
		X
		*/
			for (i=0;i<11;++i){
				if(hfp[i]+2<=hfp[i+1]){
					if(placemt.posY==-1 || placemt.posY>=hfp[i]){
						placemt.posX = i;
						placemt.posY = maxi(hfp[i],hfp[i+1]-2);
						placemt.sens = 3;
					}
				}
			}
		/*
		X X X
		    X
		*/
			for (i=0;i<10;++i){
				if(hfp[i]==hfp[i+1] && hfp[i+1]==hfp[i+2]+1){
					if(placemt.posY==-1 || placemt.posY>=hfp[i]){
						placemt.posX = i;
						placemt.posY = hfp[i+2];
						placemt.sens = 4;
					}
				}
			}

		/*
		X
		X X X
		*/
			for (i=0;i<10;++i){
				if(hfp[i]==hfp[i+1] && hfp[i+1]==hfp[i+2]){
					if(placemt.posY==-1 || placemt.posY>=hfp[i]){
						placemt.posX = i;
						placemt.posY = hfp[i+1];
						placemt.sens = 2;
					}
				}
			}
		/*
		  X
		  X
		X X
		*/
			for (i=0;i<11;++i){
				if(hfp[i]==hfp[i+1]){
					if(placemt.posY==-1 || placemt.posY>=hfp[i]){
						placemt.posX = i;
						placemt.posY = hfp[i];
						placemt.sens = 1;
					}
				}
			}


		/*placement par défaut */
			if (placemt.posY==-1){
				if(hfp[0]<=hfp[1]){min=1;}
				else {min=0;}
				for (i=1;i<11;++i){
					if(maxi(hfp[i],hfp[i+1])<hfp[min]){min=i;}
				}
				placemt.posX = min;
				placemt.posY = maxi(hfp[min],hfp[min+1]);
		 		placemt.sens = 1;
		 	}
			return placemt;

	/*
	    X
	X X X
	*/
		}else if (piece_courante==4){
						
		/*
		X X
		  X
		  X
		*/
			for (i=0;i<11;++i){
				if(hfp[i]>=hfp[i+1]+2){
					if(placemt.posY==-1 || placemt.posY>=hfp[i]){
						placemt.posX = i;
						placemt.posY = maxi(hfp[i]-2,hfp[i+1]);
						placemt.sens = 3;
					}
				}
			}

		/*
		X X X 
		X
		*/

			for (i=0;i<10;++i){
				if(hfp[i]==hfp[i+1]-1 && hfp[i+1]==hfp[i+2]){
					if(placemt.posY==-1 || placemt.posY>=hfp[i]){
						placemt.posX = i;
						placemt.posY = hfp[i];
						placemt.sens = 4;
					}
				}
			}

		/*
		    X
		X X X
		*/
			for (i=0;i<10;++i){
				if(hfp[i]==hfp[i+1] && hfp[i+1]==hfp[i+2]){
					if(placemt.posY==-1 || placemt.posY>=hfp[i]){
						placemt.posX = i;
						placemt.posY = hfp[i];
						placemt.sens = 2;
					}
				}
			}
		/*
		X
		X
		X X
		*/

			for (i=0;i<11;++i){
				if(hfp[i]==hfp[i+1]){
					if(placemt.posY==-1 || placemt.posY>=hfp[i]){
						placemt.posX = i;
						placemt.posY = hfp[i];
						placemt.sens = 1;
					}
				}
			}

			/*placement par défaut */
			if (placemt.posY==-1){
				if(hfp[0]<=hfp[1]){min=1;}
				else {min=0;}
				for (i=1;i<11;++i){
					if(maxi(hfp[i],hfp[i+1])<hfp[min]){min=i;}
				}
				placemt.posX = min;
				placemt.posY = maxi(hfp[min],hfp[min+1]);
		 		placemt.sens = 1;
		 	}
			return placemt;



	/*
	  X X
	X X
	*/
		}else if (piece_courante==5){
		    /*placement horizontal si possible */
			for (i=0;i<10;++i){
				if(hfp[i]==hfp[i+1] && hfp[i+1]==hfp[i+2]-1 ){
					if(placemt.posY == -1 || hfp[i]<=placemt.posY) {
					    placemt.posY=hfp[i];
					    placemt.posX=i;
					    placemt.sens=1;

					}
				}
			}

			/* placement vertical bien encastré sinon */
			for (i=0;i<11;++i){
				if(hfp[i]-1==hfp[i+1]){
					if(placemt.posY == -1 || hfp[i]<=placemt.posY) {
					    placemt.posY=hfp[i+1];
					    placemt.posX=i;
					    placemt.sens=2;

					}
				}
			}

			/* placement vertical basique par defaut */
            		if (placemt.posY==-1){
				if(hfp[0]<=hfp[1]-1){min=1;}
				else {min=0;}
				for (i=1;i<11;++i){
					if(maxi(hfp[i],hfp[i+1]-1)<hfp[min]){min=i;}
				}
				placemt.posX = min;
				placemt.posY = maxi(hfp[min]-1,hfp[min+1]);
		 		placemt.sens = 2;
		 	}


			return placemt;





	/*
	X X
	  X X
	*/
		}else if (piece_courante==6){
		    /*placement horizontal si possible */
			for (i=0;i<10;++i){
				if(hfp[i]-1==hfp[i+1] && hfp[i+1]==hfp[i+2] ){
					if(placemt.posY == -1 || hfp[i]<=placemt.posY) {
					    placemt.posY=hfp[i+1];
					    placemt.posX=i;
					    placemt.sens=1;

					}
				}
			}

			/* placement vertical bien encastré sinon */
			for (i=0;i<11;++i){
				if(hfp[i]==hfp[i+1]-1){
					if(placemt.posY == -1 || hfp[i]<=placemt.posY) {
					    placemt.posY=hfp[i];
					    placemt.posX=i;
					    placemt.sens=2;

					}
				}
			}

			/* placement vertical basique par defaut */
            if (placemt.posY==-1){
				if(hfp[0]>=hfp[1]-1){min=1;}
				else {min=0;}
				for (i=1;i<11;++i){
					if(maxi(hfp[i],hfp[i+1]-1)<hfp[min]){min=i;}
				}
				placemt.posX = min;
				placemt.posY = maxi(hfp[min],hfp[min+1]-1);
		 		placemt.sens = 2;
		 	}


			return placemt;

	/*
	  X
	X X X
	*/
		
		}else if (piece_courante==7){
			
		/*
		X X X
		  X
		*/
			for (i=0;i<10;++i){
				if(hfp[i]==hfp[i+1]+1 && hfp[i+1]+1==hfp[i+2]){
					if(placemt.posY==-1 || placemt.posY>=hfp[i]){
						placemt.posX = i;
						placemt.posY = hfp[i+1];
						placemt.sens = 2;
					}
				}
			}
		/*
		X
		X X
		X
		*/
			for (i=0;i<11;++i){
				if(hfp[i]==hfp[i+1]-1){
					if(placemt.posY==-1 || placemt.posY>=hfp[i]){
						placemt.posX = i;
						placemt.posY = hfp[i];
						placemt.sens = 3;
					}
				}
			}
			
		/*
		  X
		X X
		  X
		*/
			for (i=0;i<11;++i){
				if(hfp[i]==hfp[i+1]+1){
					if(placemt.posY==-1 || placemt.posY>=hfp[i]){
						placemt.posX = i;
						placemt.posY = hfp[i+1];
						placemt.sens = 4;
					}
				}
			}
			
		/*
		  X
		X X X
		*/
			
			for (i=0;i<10;++i){
				if(hfp[i]==hfp[i+1] && hfp[i]==hfp[i+2]){
					if(placemt.posY==-1 || placemt.posY>=hfp[i]){
						placemt.posX = i;
						placemt.posY = hfp[i];
						placemt.sens = 1;
					}
				}
			}
			
						
			/*placement par défaut */	
			if (placemt.posY==-1){
				if(hfp[0]<=hfp[1]){min=1;}
				else {min=0;}
				for (i=1;i<11;++i){
					if(maxi(hfp[i],hfp[i+1]-1)<hfp[min]){min=i;}
				}
				placemt.posX = min;
				placemt.posY = maxi(hfp[min],hfp[min+1]+1);
		 		placemt.sens = 3;
		 	}
			return placemt;
		}

	}else{
			placemt.piece = 0;
	        placemt.posX = 0;
	        placemt.posY = 0;
 	        placemt.sens = 0;
	}
	return placemt;
}

void * ia_thread(void *data){
    while (1){
		deplacement depla;
		depla = calcule_mouvement(field);
		if (depla.piece!=0)prepare_placement(depla);
		usleep(200);
	}	
	return NULL;
}

int prepare_placement(deplacement depla){
    plcmt = depla;
    doitPlacer = 0;
}
