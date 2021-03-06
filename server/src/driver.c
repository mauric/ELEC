

#include "../include/driver.h"


	
struct termios oldattr;	
int serial_init(const char *devname, speed_t baudrate)
{
	int fd;
	struct termios newattr;

	if((fd = open(devname, O_RDWR | O_NOCTTY)) < 0) {
		perror("Failed to open serial port");
		exit(EXIT_FAILURE);
	} else if(tcgetattr(fd, &oldattr) != 0) {
		perror("Failed to get configuration");
		exit(EXIT_FAILURE);
	}
	newattr = oldattr;

	cfsetispeed(&newattr, baudrate);
	cfsetospeed(&newattr, baudrate);
	
	// 8N1
    newattr.c_cflag &= ~PARENB;
    newattr.c_cflag &= ~CSTOPB;
    newattr.c_cflag &= ~CSIZE;
    newattr.c_cflag |= CS8;
	
	newattr.c_iflag &= ~(IGNBRK | BRKINT | PARMRK | ISTRIP
                           | INLCR | IGNCR | ICRNL | IXON);
    newattr.c_oflag &= ~OPOST;
    newattr.c_lflag &= ~(ECHO | ECHONL | ICANON | ISIG | IEXTEN);
    newattr.c_cflag &= ~(CSIZE | PARENB | HUPCL);
    newattr.c_cflag |= CS8;
    newattr.c_cc[VMIN]  = 50;
    newattr.c_cc[VTIME] = 10;

	if(tcsetattr(fd, TCSANOW, &newattr) != 0) {
		perror("Failed to set configuration");
		exit(EXIT_FAILURE);
	}

	tcflush(fd,TCIOFLUSH);

	return fd;
}
unsigned char checksum(unsigned char * trame){
	signed char res=0x00;
	unsigned char taille=*(trame+1)<<8|*(trame+2);//printf("la taille vaut %x",taille);
	int a=1;
	for(unsigned char taill=taille;taill>0;taill=taill-1){
		
		res+=trame[a+2];
		a++;						}
	return ((0xFF-res)&0x0000FF);
}



// fonction write de BB vers capteur
void writeZ(capteur * c,int fd){
	
	unsigned char * message =(unsigned char*)malloc((21));
	message[0]=0x7e;
	message[1]=0x00;
	message[2]=17;
	message[3]=0x10;
	message[4]=0x01;
	for(int i=0;i<8;i++){
		message[5+i]=c->z->adresse[i];}
	message[13]=c->z->adresse_dynamique[0];
	message[14]=c->z->adresse_dynamique[1];
	message[17]=c->id;
	message[18]=0;
	message[19]=c->freq;
	message[20]=checksum(message);
	for(int i=0;i<21;i++){fprintf(stderr, "%02X ",message[i]);}
	printf("\n");
	write(fd,message,21);
	free(message);
}

//fonction read de BB
void readZ(int fd,capteur * liste){

unsigned char rep[256];
int n=read(fd, rep, 3);
int N=rep[1]<<8|rep[2];//on recupère la taille
n=read(fd,rep+3, N+1);
(void)n;
//on affiche la trame avant l'analyse
for (int i=0;i<N+4;i++) printf("%02X ", rep[i]);
printf("\n");
unsigned char type=rep[3];// quelle type de trame est ce ?
switch(type){
	case 0x90:
		//printf("On a reçu une info: %02X \n",rep[15]);
		refreshState(rep,liste);
		break;
	case 0x95:
		printf("\nUn nouveau s'est connecté %02X %02X\n",rep[8],rep[9]);
		break;
	case 0x8b:
		if(rep[8]==0x00)
		printf("\nMessage transmis avec succès\n");
		else printf("\nProblème dans la transmission du message\n");
		break;
	default:
		printf("\nmessage dans le default\n");
		break;

		}

}

//liste de capteur et la trame
void refreshState(unsigned char * trame,capteur * liste){
	
if(trame[15]==0x01){
	liste[0].state=trame[16];
	liste[0].info[0]=trame[17];
	liste[0].info[1]=trame[18];
		}	
if(trame[15]==0x02){
	liste[1].state=trame[16];
	liste[1].info[0]=trame[17];
	liste[1].info[1]=trame[18];
	liste[1].info[2]=trame[19];
			
		}

if(trame[15]==0x03){
	liste[2].state=trame[16];
	liste[2].info[0]=trame[17];
	liste[2].info[1]=trame[18];

			
		}	
if(trame[15]==0x04){
	liste[3].state=trame[16];
	liste[3].info[0]=trame[17];
	liste[3].info[1]=trame[18];

			
		}


		





}



/***** cette fonction permet de convertir les données des capteurs en XML *****/
const char * toXML(capteur *liste){
	static char xml[0x1000];
	int i;
	sprintf(xml,"<capteurs>");

	for(i=0;i<4;i++){
	   	if(i==1){
	
	        	sprintf(xml,"%s<%s><state>%d</state><val>%d</val><val>%d</val><val>%d</val></%s>",xml,"acc",(int)liste[i].state,(int)liste[i].info[0],(int)liste[i].info[1],(int)liste[i].info[2],"acc");
			
			}
	    	else if(i==0){
			int virg=0;
			if(liste[i].info[1]==0x50)virg=5;
			sprintf(xml,"%s<%s><state>%d</state><val>%d</val><val>%d</val></%s>",xml,"temp",liste[i].state,liste[i].info[0],virg,"temp");
   	   		}
		else if(i==2){
			int exp=liste[i].info[1]>>4 & 0x0F;
			int mant=((liste[i].info[0]&0x0F)| liste[i].info[1]<<4) &0x0F;
			int lux=1;
			lux=(int)(pow(2,(double)exp)*(double)mant*0.045);	
			sprintf(xml,"%s<%s><state>%d</state><val>%d</val><val>%d</val></%s>",xml,"lum",liste[i].state,lux*1,0,"lum");
   	   		 }
		else if(i==3){
			int val1=((liste[i].info[0]*100)/0xFF)&0x000000FF;
			int val2=((liste[i].info[1]*100)/0xFF)&0x000000FF;
			sprintf(xml,"%s<%s><state>%d</state><val>%d</val><val>%d</val></%s>",xml,"can",liste[i].state,val1,val2,"can");
   				}

			 }
	sprintf(xml,"%s</capteurs>",xml);
	return xml;
}







void get_lux(char * rep,capteur * liste){
if(liste[2].state==0x11){
	rep[4]=1;
	rep[5]=liste[2].info[0];
	rep[6]=liste[2].info[1];
	rep[7]=';';
			}
else {rep[4]=0;rep[5]=0;rep[6]=0;rep[7]=';';}
					}



void get_can(char * rep,capteur * liste){
if(liste[3].state==0x11){
	rep[13]=1;
	rep[14]=liste[4].info[0];
	rep[15]=liste[4].info[1];
	rep[16]=';';
			}
else {rep[13]=0;rep[14]=0;rep[15]=0;rep[16]=';';}
					}
	
void get_acc(char * rep,capteur * liste){
if(liste[1].state==0x11){
	rep[8]=1;
	rep[9]=liste[1].info[0];
	rep[10]=liste[1].info[1];
	rep[11]=liste[1].info[2];
	rep[12]=';';
			}
else {rep[8]=0;rep[9]=0;rep[10]=0;rep[11]=0;rep[12]=';';}
					}


void get_temp(char * rep,capteur * liste){
if(liste[0].state==0x11){
	rep[0]=1;
	rep[1]=liste[0].info[0];
	rep[2]=liste[0].info[1];
	rep[3]=';';
	
			}
else {rep[0]=0;rep[1]=0;rep[2]=0;rep[3]=';';}
					}	






	

