//------------------------------------------------------------------------//
//  PROJET ELECTRONIQUE
// File : fb.c
// Comments : traitement d'images
//------------------------------------------------------------------------//


#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <errno.h>
#include <string.h>
#include <getopt.h>             /* getopt_long() */
#include <unistd.h>
#include <stdint.h>
#include <sys/mman.h>
#include <sys/ioctl.h>
#include <linux/fb.h>
#include <math.h> 
#include <time.h> 
#include<stdbool.h>



typedef void (*ProcessFunc)(uint8_t *, int, int);

/* Device Name like /dev/fb */
#define FBNAME	"/dev/fb0"

/* fixed screen information */
struct fb_fix_screeninfo fix_info;

/* configurable screen info */
struct fb_var_screeninfo var_info;

/* The frame buffer memory pointer */
uint8_t *framebuffer;

/*
 * Macro to pack the pixel based on the rgb bit offset.
 * We compute each color value based on bit length
 * and shift it to its corresponding offset in the pixel.
 * each color component is 8 bits long
 *
 * For example: Considering a RGB565, the formula will
 * expand as:-

 * Red len=5, off=11 : Green len=6, off=6 : Blue len=5, off=0
 * pixel_value = ((red >> (8 - 5) << 11)|
 *       ((green >> (8 - 6) << 6) |
 *      ((blue >> (8 - 5) << 0)
 */
#define RGB(r,g,b) ( \
        (((r) >> (8-var_info.red.length)) << var_info.red.offset) | \
        (((g) >> (8-var_info.green.length)) << var_info.green.offset) | \
        (((b) >> (8-var_info.blue.length)) << var_info.blue.offset) \
)

#define SET_PIXEL(x,y,color) (((short*)framebuffer)[(x)+(y)*fix_info.line_length/2]=(color))
#define GRAYLEVEL 256
#define BACKGROUND 0
#define FOREGROUND 255
#define NOMBRE_VOISINS 8
#define PI 3.1416

/* function to a filled rectangle at position (x,y), width w and height h */
void rect_fill(int x,int y, int w, int h, unsigned short color)
{
	int i, j;
	for (i=0;i<w;i++) {
		for (j=0;j<h;j++) {
			SET_PIXEL(x+i,y+j,color);
		}
	}
}

/*function to find the minVal into an array, return 0 like error*/
uint8_t find_min_label(const uint8_t* vector, int taille){
	uint8_t minVal; //minVal label
	int flag = 0;
	for (int i = 0; i < taille; ++i){
		if(vector[i] != 0){
			if (flag == 0){
				flag = 1;
				minVal = vector[i];
			}
			if(vector[i]<minVal)minVal=vector[i];
		}
	}
	return minVal;
}

uint8_t Y[640][480]; //composant Y en image
double YH[640][480]; //filtrage Horizontal de la composant Y
double YV[640][480]; //filtrage Vertical de la composant Y
double N[640][480];  //Matrice Norme
uint8_t E[640][480];  //Matrice des etiquettes
uint8_t E2[640][480];  //Matrice des etiquettes connectes
uint8_t S[640][480];  //Matrice image seuil
  int aire[255]={0};
  int perimetre[255]={0};//hace un memset

/***************************************************************************
 * decoding stuff
 ***************************************************************************/
#define bounds(m,M,x) ((x)>M ? M : (x)<(m) ? m : (x)) //macro for test value doesnt get out of bounds
#define min(a,b) ((a)>b ? b : a)
#define max(a,b) ((a)<b ? b : a)
static void process_image_yuv422 (uint8_t * videoFrame, int width, int height){
	//fprintf(stderr, "ICI\n");
	/* A COMPLETER  DONE*/
	
/*	uint8_t Y[width][height]; //composant Y en image
	double YH[width][height]; //filtrage Horizontal de la composant Y
	double YV[width][height]; //filtrage Vertical de la composant Y
	double N[width][height];  //Matrice Norme
	uint8_t E[width][height];  //Matrice des etiquettes
	uint8_t E2[width][height];  //Matrice des etiquettes connectes
	uint8_t S[width][height];  //Matrice image seuil*/

	uint8_t tmp;			  //variable temporale	
	signed int i = 1; //
	int x=0; /* Loop variable */// x,y; -> coordones dans l'image
	int y =0;
	
  //------------------------------------------------------------------------//
  //              Recupere Composant Y
  //------------------------------------------------------------------------//

	for ( y = 0; y < height; y++) {
		for (x = 0; x < width; x++) { 
			/* A COMPLETER : DONE */
			Y[x][y]=videoFrame[i]; //recupêration de image blanc et noir sur la matrice Y
			tmp = Y[x][y];
			
			//affichage  
			 SET_PIXEL(x,y,RGB(tmp,tmp,tmp));  
			i+=2;
		}
	}
	
  //------------------------------------------------------------------------//
  //               Detection de bords
  //------------------------------------------------------------------------//

	//calcul de la convolution avec filtre de prewitt : rappel pas toucher le borde
	for ( y = 1; y < height-1; y++) {
		for (x = 1; x < width-1; x++) { 
			//faire attention les bordes ne sont pas initialises
			if((x - 1) > 0 && (y - 1) > 0 && (x + 1) < width && (y + 1) < height){
             YH[x][y] = (-Y[x-1][y-1]+Y[x+1][y-1]-Y[x-1][y]+Y[x+1][y]-Y[x-1][y+1]+Y[x+1][y+1]);  //filtrage horizontal prewitt
             YV[x][y] = (-Y[x-1][y-1]-Y[x][y-1]-Y[x+1][y-1]+Y[x-1][y+1]+Y[x][y+1]+Y[x+1][y+1]);  //filtrage vertical prewitt
         	}
		}
	}

	//calcul de la matrice norme de la matrice Y
	double med = 0;
	for (y = 0; y < height; y++) {
		for (x = 0; x < width; x++) { //chaque é elements parce on est en 422
			N[x][y] = (YH[x][y]*YH[x][y] + YV[x][y]*YV[x][y]);
			med += N[x][y];
		}
	}
	med = med / (width * height); //vqleur moyen


	//seuillage detection bords -  traitement sur image N (norme)
	double seuil = med * 4;
	for (y = 0; y < height; y++) {
		for (x = 0; x < width; x++) { //chaque é elements parce on est en 422
             if (N[x][y]<seuil)
             {   		
             	SET_PIXEL(x+width,y,RGB(0,0,0));
             }else{
             	SET_PIXEL(x+width,y,RGB(255,255,255));
             }
		}
	}

  //------------------------------------------------------------------------//
  //               Algorithme de Otsu
  //------------------------------------------------------------------------//

		//algorithme pour trouver seuillage
	  int hist[GRAYLEVEL];
	  double prob[GRAYLEVEL], omega[GRAYLEVEL]; /* prob of graylevels */
	  double myu[GRAYLEVEL];   /* mean value for separation */
	  double max_sigma, sigma[GRAYLEVEL]; /* inter-class variance */
	  double threshold; /* threshold for binarization */
	  //printf("Otsu's binarization process starts now.\n");
	  
	  /* Histogram generation */
	  for (i = 0; i < GRAYLEVEL; i++) hist[i] = 0; //init

	  for (y = 0; y < height; y++)
	    for (x = 0; x < width; x++) {
	      hist[Y[x][y]]++;  //TODO faire bien le calcul
	    }

	 /* calculation of probability density */
	  for ( i = 0; i < GRAYLEVEL; i ++ ) {
	    prob[i] = (double)hist[i] / (width * height);
	  }
	 /* omega & myu generation */
	  omega[0] = prob[0];//Set up initial \omega_i(0) and \mu_i(0)
	  myu[0] = 0.0;       /* 0.0 times prob[0] equals zero */
	  for (i = 1; i < GRAYLEVEL; i++) {//Step through all possible thresholds t = 1 \ldots maximum intensity 
	    omega[i] = omega[i-1] + prob[i];//Update \omega_i and \mu_i
	    myu[i] = myu[i-1] + i*prob[i];//Compute \sigma^2_b(t)
	  }
	 	 /*
		Desired threshold corresponds to the maximum \sigma^2_b(t)
	 	sigma maximization
		sigma stands for inter-class variance 
	     and determines optimal threshold value 
		*/
	  threshold = 0;
	  max_sigma = 0.0;
	  for (i = 0; i < GRAYLEVEL-1; i++) {
	    if (omega[i] != 0.0 && omega[i] != 1.0)
	      sigma[i] = pow(myu[GRAYLEVEL-1]*omega[i] - myu[i], 2) / 
		(omega[i]*(1.0 - omega[i]));
	    else
	      sigma[i] = 0.0;
	    if (sigma[i] > max_sigma) {
	      max_sigma = sigma[i];
	      threshold = i;

	    }
	 }
	 /*Afficher la segmentation*/
	for (y = 0; y < height; y++) {
		for (x = 0; x < width; x++) { //chaque é elements parce que on est en codif 422
	         if (Y[x][y]<threshold)
	         {
	         	S[x][y] = 255;//blanc
	         //	SET_PIXEL(x,y+height,RGB(255,255,255));//TODO : verifier que c'est vraiment S ce qu'on voit
	         }else{
	         	S[x][y] = 0;
	         //	SET_PIXEL(x,y+height,RGB(0,0,0));
	         }
		}
	}

  //------------------------------------------------------------------------//
  //              Algorithme Two Pass - Detection de regions
  //------------------------------------------------------------------------//

	/*
	Algorithme Two-Pass
	*/

	/*              x
	 0 1 2       3       4           5 

	 0   G R G       R       G           R 
	 1   B G B [x-1][y-1] [x][y-1] [x+1][y-1] 
	 y   2   G R G [x-1][y]      o       R 
	 3   B G B G B G 

	 */

	uint8_t currentLabel = 1; //jamais 0
	uint8_t voisins[NOMBRE_VOISINS];
	uint8_t etiquettes[NOMBRE_VOISINS];
	uint8_t id[100];
	//memset(id, 0, 0, sizeof(uint8_t)*100);
	for (int i = 0; i < 100; ++i)
	{
		id[i]=0;
	}
	id[0] = currentLabel;
	for (int i = 0; i < NOMBRE_VOISINS; ++i) {
		voisins[i] = 0;
		etiquettes[i] = 0;
	}


	for (y = 0; y < height; y++) {
		for (x = 0; x < width; x++) { 
			E[x][y]=0;//structure with dimensions of data, initialized with the value of Background
			E2[x][y]=0;
		}
	}

	/*
	 Partie I de reconnaissance de formes
	 parcourir l'image
	 */
	 int nombre_id = 1;
	for (y = 0; y < height; y++) {
		for (x = 0; x < width; x++) { //chaque é elements parce que on est en codif 422
			if (S[x][y] != BACKGROUND) {
				//recuperer voisins
				if ((x - 1) > 0) {
					voisins[0] = S[x - 1][y];     //ouest
				} else {
					voisins[0] = 0;             //ouest
				}
				if ((x - 1) > 0 && (y - 1) > 0) {
					voisins[1] = S[x - 1][y - 1];   //nord-ouest    
				} else {
					voisins[1] = 0;             //nord-ouest
				}
				if ((y - 1) > 0) {
					voisins[2] = S[x][y - 1];     //nord
				} else {
					voisins[2] = 0;             //nord
				}
				if ((x + 1) < width && (y - 1) > 0) {
					voisins[3] = S[x + 1][y - 1];   //nord-est
				} else {
					voisins[3] = 0;             //nord-est
				}

				if (voisins[0] == 0 && voisins[1] == 0 && voisins[2] == 0
						&& voisins[3] == 0) {
					/*donner une nouvelle etiquette*/
					currentLabel = currentLabel + 1;
					E[x][y] = currentLabel;
					id[nombre_id] = currentLabel;
					nombre_id++;
					//printf("new label  %u\n", E[x][y]);

				}else 
					if(voisins[0] == 255 && voisins[1] == 0 && voisins[2] == 0
						&& voisins[3] == 0) {
					/*donner l'etiquette de l'elements precedente (ouest)*/
						if ((x - 1) > 0) {
								E[x][y] = E[x - 1][y];
							}
					//printf("%s\n", "meme region"); //TODO eliminar
				} else {
					for (int i = 0; i < NOMBRE_VOISINS; ++i) {
						if (voisins[i] != BACKGROUND) {
							//recuperer etiquettes
							if ((x - 1) > 0) {
								etiquettes[0] = E[x - 1][y];
							} else {
								etiquettes[0] = 0;
							}
							if ((x - 1) > 0 && (y - 1) > 0) {
								etiquettes[1] = E[x - 1][y - 1];
							} else {
								etiquettes[1] = 0;
							}
							if ((y - 1) > 0) {
								etiquettes[2] = E[x][y - 1];
							} else {
								etiquettes[2] = 0;
							}
							if ((x + 1) < width && (y - 1) > 0) {
								etiquettes[3] = E[x + 1][y - 1];
							} else {
								etiquettes[3] = 0;
							}
						} else {
							etiquettes[i] = 0; //ecraseer a 0 etiquettes qui sont pas importants dans vecteur
						}
					} //fin recuperer etiquettes

					//chercher la etiquettes plus petit
					E[x][y] = find_min_label(etiquettes, NOMBRE_VOISINS);
					//printf("etiquettes plus petit %u\n", E[x][y]);
				}
			} //fin verification si pixel est fond - figure
	//	SET_PIXEL(x + width, y , RGB(E[x][y],80,E[x][y]));
		}
	} //fin boucle x,y partie 1 detection formes  



	/* 
		Partie II de reconnaissance de regions
	Parcour de matriz avec etiquettes E
	             x
	 0 1 2       3       4           5 

	 0         R       G           R 
	 1    [x-1][y-1]  [x][y-1] 	   [x+1][y-1] 
	 y    [x-1][y]      o   	   [x+1][y] SS
	 3    [x-1][y+1]    [x][y+1] 	[x+1][y+1]	

	 */

	uint8_t autour[8] = {0,0,0,0,0,0,0,0};
	uint8_t label_min;
	/*faire une deuxieme boucle pour ecraser valeur des etiquettes*/
	for (y = 0; y < height; y++) {
		for (x = 0; x < width; x++) { //chaque é elements parce que on est en codif 422
			if (E[x][y] != BACKGROUND) {
				//recuperer autour
				if ((x - 1) > 0) {
					autour[0] = E[x - 1][y];     //ouest
				} else {
					autour[0] = 0;             //ouest
				}
				if ((x - 1) > 0 && (y - 1) > 0) {
					autour[1] = E[x - 1][y - 1];   //nord-ouest    
				} else {
					autour[1] = 0;             //nord-ouest
				}
				if ((y - 1) > 0) {
					autour[2] = E[x][y - 1];     //nord
				} else {
					autour[2] = 0;             //nord
				}
				if ((x + 1) < width && (y - 1) > 0) {
					autour[3] = E[x + 1][y - 1];   //nord-est
				} else {
					autour[3] = 0;             //nord-est
				}
				if((x+1)<width ){
					autour[4] = E[x+1][y];
				}else{
					autour[4] = 0;
				}
				if((x+1)<width && (y+1)<height){
					autour[5] = E[x+1][y+1];
				}else{
					autour[5] = 0;					
				}
				if((y+1)<height){
					autour[6] = E[x][y+1] ;
				}else{
					autour[6] = 0;					
				}
				if((x-1) > 0 && (y + 1) < height){
					autour[7] = E[x-1][y+1];				
				}else{
					autour[7] = 0;					
				}
				//printf("%x\n","ici" );// TODO : effacer message 
				label_min = find_min_label(autour,8);//*(255/5);



				 /*reemplacer autour du pixel actuel*/
				//recuperer autour
				if ((x - 1) > 0 && E[x - 1][y]!= 0) {
					 E2[x - 1][y] = label_min;     //ouest
				} 
				if ((x - 1) > 0 && (y - 1) > 0 && E[x - 1][y - 1] != 0) {
					E2[x - 1][y - 1]= label_min;   //nord-ouest    
				} 
				if ((y - 1) > 0 && E[x][y - 1] != 0) {
					E2[x][y - 1]= label_min;     //nord
				}
				if ((x + 1) < width && (y - 1) > 0 && E[x + 1][y - 1] != 0) {
					E2[x + 1][y - 1]= label_min;   //nord-est
				} 
				if((x+1)<width && E[x+1][y] != 0 ){
					E2[x+1][y]= label_min;
				}
				if((x+1)<width && (y+1)<height && E[x+1][y+1] != 0){
					E2[x+1][y+1]= label_min;
				}
				if((y+1)<height && E[x][y+1] != 0){
					 E2[x][y+1]= label_min ;
				}
				if((x-1) > 0 && (y + 1) < height && E[x-1][y+1] != 0){
					 E2[x-1][y+1]= label_min;				
				}
			 }//fin verification si background

		}//fin boucle int
	}//fin boucle ext

	/*boucle dans l'autre sens			*/
	for (y = height; y > 0; y--) {
		for (x = width; x > 0; x--) { //chaque é elements parce que on est en codif 422
			if (E2[x][y] != BACKGROUND) {
				//recuperer autour
				if ((x - 1) > 0) {
					autour[0] = E2[x - 1][y];     //ouest
				} else {
					autour[0] = 0;             //ouest
				}
				if ((x - 1) > 0 && (y - 1) > 0) {
					autour[1] = E2[x - 1][y - 1];   //nord-ouest    
				} else {
					autour[1] = 0;             //nord-ouest
				}
				if ((y - 1) > 0) {
					autour[2] = E2[x][y - 1];     //nord
				} else {
					autour[2] = 0;             //nord
				}
				if ((x + 1) < width && (y - 1) > 0) {
					autour[3] = E2[x + 1][y - 1];   //nord-est
				} else {
					autour[3] = 0;             //nord-est
				}
				if((x+1)<width ){
					autour[4] = E[x+1][y];
				}else{
					autour[4] = 0;
				}
				if((x+1)<width && (y+1)<height){
					autour[5] = E[x+1][y+1];
				}else{
					autour[5] = 0;					
				}
				if((y+1)<height){
					autour[6] = E[x][y+1] ;
				}else{
					autour[6] = 0;					
				}
				if((x-1) > 0 && (y + 1) < height){
					autour[7] = E[x-1][y+1];				
				}else{
					autour[7] = 0;					
				}
				//printf("%x\n","ici" );// TODO : effacer message 
				label_min = find_min_label(autour,8);//*(255/5);



				 /*reemplacer autour du pixel actuel*/
				//recuperer autour
				if ((x - 1) > 0 && E2[x - 1][y]!= 0) {
					 E2[x - 1][y] = label_min;     //ouest
				} 
				if ((x - 1) > 0 && (y - 1) > 0 && E2[x - 1][y - 1] != 0) {
					E2[x - 1][y - 1]= label_min;   //nord-ouest    
				} 
				if ((y - 1) > 0 && E2[x][y - 1] != 0) {
					E2[x][y - 1]= label_min;     //nord
				}
				if ((x + 1) < width && (y - 1) > 0 && E2[x + 1][y - 1] != 0) {
					E2[x + 1][y - 1]= label_min;   //nord-est
				} 
				if((x+1)<width && E2[x+1][y] != 0 ){
					E2[x+1][y]= label_min;
				}
				if((x+1)<width && (y+1)<height && E2[x+1][y+1] != 0){
					E2[x+1][y+1]= label_min;
				}
				if((y+1)<height && E2[x][y+1] != 0){
					 E2[x][y+1]= label_min ;
				}
				if((x-1) > 0 && (y + 1) < height && E2[x-1][y+1] != 0){
					 E2[x-1][y+1]= label_min;				
				}
			 }//fin verification si background
			// fprintf(stderr, "%d\n", label_min);
			 //changer couleur

		}//fin boucle int
	}//fin boucle ext


	//fin partie II de l'algorithme
	for (y = 0; y < height; y++) {
		for (x = 0; x < width; x++) { //c
			if (E2[x][y] != BACKGROUND) {
	      		SET_PIXEL(x + width, y + height, RGB(3*E2[x][y]+2,20*E2[x][y],200-10*E2[x][y]));			 
			}else{
				SET_PIXEL(x + width, y + height, RGB(0,0,0));
				}
			}
  }
  //------------------------------------------------------------------------//
  //               Detection-Formes - trouver un circle
	//------------------------------------------------------------------------//
 //  //usar memoria dinámica con la talle de currenbt label
  for (int i = 0; i < 255; ++i)
  {
  	aire[i]=0;
  	perimetre[i]=0;
  }

	  //calcul de l'aire de formes
	  for (y = 0; y < height; y++) {
	    for (x = 0; x < width; x++) { //chaque é elements parce que on est en codif
	      //calcular el area de figuras figura 1
	      if (E[x][y] != BACKGROUND) {
	      aire[E2[x][y]]++; //tableu avec valeurs de aire
	      }
	    }
	  }
	  //calcul du perimetre
  for ( y = 1; y < height-1; y++) {
		for (x = 1; x < width-1; x++) {
			//faire attention les bordes ne sont pas initialises??
        if (E[x][y] != BACKGROUND) {
			if((x - 1) >= 0 && (y - 1) >= 0 && (x + 1) <= width && (y + 1) <= height){
          if(E[x - 1][y] == 0 || E[x][y - 1] == 0 || E[x + 1][y] == 0 || E[x][y + 1] == 0){
            	perimetre[E[x][y]]++;  //un pixel connection 4
            	SET_PIXEL(x,y+height, RGB(0,51,204));
          }else{
          	SET_PIXEL(x,y+height, RGB(0,0,0));
          }
        }
      }else{
  		SET_PIXEL(x,y+height, RGB(0,0,0));

      }
	 }
	}


 // RAPPEL  //decision sobre la equivalencia de la format
 //  //superficie * 4 * pi / perimetro ^ 2 = aprox(1)

   double caracteristique[255];// = {0.0};
   double caracteristique2[255];// = {0.0};
   //memset(caracteristique, 0, 0.0, sizeof(double)*255);
   double detection[255];// = {0.0};
   //memset(detection, 0, 0.0, sizeof(double)*255);
	for (int i = 0; i < 255; ++i)
	{
		caracteristique[i]=0.0;
		caracteristique2[i]=0.0;
		detection[i]=0.0;
	}

  for (size_t element = 0; element < 255; element++) {
  	//calcul de une caracteristique pour diferencier les formes
    caracteristique[element] = (aire[element] * 4 * PI )/ (perimetre[element] * perimetre[element]);
    	if(caracteristique[element] > 1.4 && caracteristique[element] < 2.2 ){
          //dar color a un circulo
	      //donner couleur au  cercle
	      for (y = 0; y < height; y++) {
	        for (x = 0; x < width; x++) { //chaque é elements parce que on est en
	        	if(E2[x][y] == element)SET_PIXEL(x,y+height, RGB(0,200,204));
	   	     }
	        }

		}
  }

  // for (size_t element = 0; element < 255; element++) {
  // 	//calcul de une caracteristique pour diferencier les formes
  //   caracteristique2[element] = (aire[element] * 4 * PI )/ (perimetre[element] * perimetre[element]);
  //   	if(caracteristique[element] > 1.4 && caracteristique[element] < 2.2 ){
  //         //dar color a un circulo
	 //      //donner couleur au  cercle
	 //      for (y = 0; y < height; y++) {
	 //        for (x = 0; x < width; x++) { //chaque é elements parce que on est en
	 //        	if(E2[x][y] == element)SET_PIXEL(x,y+height, RGB(0,200,204));
	 //   	     }
	 //        }

		// }
  // }
      



} //end of funcion process_image_yuv422 

/*              x
		0 1 2 3 4 5 6 7
		
	0	G R G R G R G R
	1	B G B G B G B G
y	2	G R G R G R G R
	3	B G B G B G B G
	4	G R G R G R G R
	5	B G B G B G B G

 */

static void process_image_raw12(uint8_t * videoFrame, int width, int height)
{
	//int    x,y;
	//unsigned char R,G,B,Y,U,V;
	//unsigned int a, b, c;
	//unsigned short *f=(unsigned short*)videoFrame;
	
	//int i = 0;
	// for (y = 1; y < height-1; y++) {
	// 	for (x = 1; x < width-1; x++) {

			
	// 	}
	// }

}
/***************************************************************************
 * main
 ***************************************************************************/
typedef enum 
{      
	PIX_FMT_YUV420P,
	PIX_FMT_RGB565,
	PIX_FMT_RGB32,
	PIX_FMT_YUV422,
	PIX_FMT_RAW12
} pix_fmt;

//needed to parse command line arguments with getopt_long
static const char short_options [] = "p:w:h";

//also needed to parse command line arguments with getopt_long
static const struct option
long_options [] = 
{
	{ "pixel-format",required_argument,      NULL,           'p' },
	{ "window-size", required_argument,      NULL,           'w' },
	{ "help",        no_argument,            NULL,           'h' },
	{ 0, 0, 0, 0 }
};

static void usage (FILE *fp, int argc, char **argv)
{
	fprintf (fp,
				"Usage: %s [options]\n\n"
				"Options:\n"
				"-p | --pixel-format   number          Pixel Format \n"
				"                                      (0 = YUV420)\n"
				"                                      (1 = RGB565 )\n"                 
				"                                      (2 = RGB32  )\n"  
				"-w | --window-size    <640*480|       Video size\n"
				"                       320*240>\n"
				"-h | --help                           Print this message\n"
				"\n",
				argv[0]);
}

int main(int argc, char *argv[])
{
    int size;

	int					fbd;			/* Frame buffer descriptor */

	int                 width                = 640;
	int                 height               = 480;
	int                 index;
	int                 c;
	pix_fmt             pixel_format = PIX_FMT_YUV422;
	ProcessFunc			proc_func=process_image_yuv422;
	
	int					quit=0;
	//-------------------------------------add
	int x,y;
	for (y = 0; y < height; y++) {
		for (x = 0; x < width; x++) { 
			E[x][y]=0;//structure with dimensions of data, initialized with the value of Background
			E2[x][y]=0;
		}
	}


    srand(time(NULL)); // initialisation de rand

    //----------------------------------

	while(1) {
		c = getopt_long (argc, argv, short_options, long_options, &index);

		if (-1 == c)
			break; //no more arguments

		switch (c) {
			case 0: // getopt_long() flag
				break;

			case 'p':
				pixel_format = (pix_fmt) atoi(optarg);
       			switch(pixel_format) {
				case PIX_FMT_YUV422:
					proc_func=process_image_yuv422;
					break;
				case PIX_FMT_RAW12:
					proc_func=process_image_raw12;
					break;
				default:
					fprintf(stderr, "\n\nError: Pixel format NOT supported\n\n");
					exit (EXIT_FAILURE);
				}
				break;

			case 'w':
				sscanf(optarg,"%d*%d",&width,&height);
				fprintf(stderr,"window size %d*%d\n",width,height);
				break;

			case 'h':
				usage (stdout, argc, argv);
				exit (EXIT_SUCCESS);

			default:
				usage (stderr, argc, argv);
				exit (EXIT_FAILURE);
		}
	}

    /* Open the framebuffer device in read write */
    fbd = open(FBNAME, O_RDWR);
    if (fbd < 0) {
        fprintf(stderr, "Unable to open %s.\n", FBNAME);
        return EXIT_FAILURE;
    }

    /* Do Ioctl. Retrieve fixed screen info. */
    if (ioctl(fbd, FBIOGET_FSCREENINFO, &fix_info) < 0) {
        fprintf(stderr, "get fixed screen info failed: %s\n",
        	strerror(errno));
        close(fbd);
        return EXIT_FAILURE;
    }

    /* Do Ioctl. Get the variable screen info. */
	if (ioctl(fbd, FBIOGET_VSCREENINFO, &var_info) < 0) {
        fprintf(stderr, "Unable to retrieve variable screen info: %s\n",
	    	strerror(errno));
        close(fbd);
        return EXIT_FAILURE;
    }

    /* Print some screen info currently available */
    fprintf(stderr, "Screen resolution: (%dx%d)\n",var_info.xres,var_info.yres);
	fprintf(stderr, "x offset, y offset : %d, %d\n",var_info.xoffset,var_info.yoffset);
    fprintf(stderr, "Line width in bytes %d\n", fix_info.line_length);
    fprintf(stderr, "bits per pixel : %d\n", var_info.bits_per_pixel);
    fprintf(stderr, "Red: length %d bits, offset %d\n",var_info.red.length,var_info.red.offset);
	fprintf(stderr, "Green: length %d bits, offset %d\n",var_info.red.length, var_info.green.offset);
    fprintf(stderr, "Blue: length %d bits, offset %d\n",var_info.red.length,var_info.blue.offset);
	/* Calculate the size to mmap */
	size=fix_info.line_length * var_info.yres;

	/* Now mmap the framebuffer. */
	framebuffer = mmap(NULL, size, PROT_READ | PROT_WRITE, MAP_SHARED, fbd,0);
    if (framebuffer == NULL) {
        fprintf(stderr, "mmap failed:\n");
        close(fbd);
        return EXIT_FAILURE;
    }

    fprintf(stderr, "framebuffer mmap address=%p\n", framebuffer);
    fprintf(stderr, "framebuffer size=%d bytes\n", size);

/*    rect_fill(50,20,100,50,RGB(0xEE,0x55,0x76));
*/
    
    uint8_t * videoFrame = (uint8_t*) malloc (width*height*2);
	if (!videoFrame) {
		fprintf(stderr, "could not allocate buffer for video frame (%d bytes required)\n", width*height*2);
		exit (EXIT_FAILURE);
	}
	
    while (!quit)
    {
		uint8_t *ptr=videoFrame;
		
		int size=width*height*2;
//		fprintf(stderr, "LA size=%d, ptr=%x\n",size,ptr);
		while(size) {
			int ret = read(STDIN_FILENO, ptr , size);
//			fprintf(stderr, "READ size=%d, ptr=%x\n",size,ptr);
			ptr+=ret;
			size-=ret;
		}
		
		proc_func(videoFrame, width, height);

    }

    /* Release mmap. */
    munmap(framebuffer,0);
    close(fbd);
    return EXIT_SUCCESS;
}

