/*
 * zigbee.c
 *
 *  Created on: 26 mai 2016
 *      Author: a3quirat
 */
#include <fcntl.h>

#include "sys/alt_irq.h"
#include "sys/ioctl.h"
#include "sys/alt_errno.h"

#include "altera_avalon_uart.h"
#include "altera_avalon_uart_regs.h"



#include "zigbee.h"



struct termios oldattr;
int serial_init(void)
{
	int fd;
	struct termios newattr;

	if((fd = open("/dev/uart_0", O_RDWR | O_NOCTTY)) < 0) {
		perror("Failed to open serial port");
		exit(EXIT_FAILURE);
	}

    newattr.c_cflag &= ~PARENB;
    newattr.c_cflag &= ~CSTOPB;
    newattr.c_cflag &= ~CSIZE;
    newattr.c_cflag |= CS8;

	
    newattr.c_oflag &= ~OPOST;
    newattr.c_lflag &= ~(ECHO | ECHONL | ICANON | ISIG | IEXTEN);
    newattr.c_cflag &= ~(CSIZE | PARENB | HUPCL);
    newattr.c_cflag |= CS8;
    newattr.c_cc[VMIN]  = 50;
    newattr.c_cc[VTIME] = 10;

	return fd;
}
unsigned char checksum(unsigned char * trame){
	signed char res=0x00;
	unsigned char taille=*(trame+1)<<8|*(trame+2);
	 int a=1;
	 unsigned char taill;
	for(taill=taille;taill>0;taill=taill-1){
		res+=trame[a+2];
		a++;						}
	return ((0xFF-res)&0x0000FF);
}



void writeZ(capteur * c,int fd){
	unsigned char taille;
	if(c->id==2)taille=3;
	else taille =2;
	unsigned char * message =(unsigned char*)malloc((taille+21));
	message[0]=0x7e;
	message[1]=0x00;
	message[2]=taille+17;
	message[3]=0x10;
	message[4]=0x01;
	int i;
	for(i=0;i<8;i++){
	message[5+i]=c->z->adresse[i];}
	message[13]=c->z->adresse_dynamique[0];
	message[14]=c->z->adresse_dynamique[1];

	switch (c->id){
		case 1:
			printf("\nEnvoi temp  \n");
			message[17]=c->id;
			message[18]=c->state;
			message[19]=c->info[0];
			message[20]=c->info[1];
			break;
		case 2:
			printf("\nEnvoi data Accel  \n");
			message[17]=c->id;
			message[18]=c->state;
			message[19]=c->info[0];
			message[20]=c->info[1];
			message[21]=c->info[2];
			break;
		case 3:
			printf("\nEnvoi data lux  \n");
			message[17]=c->id;
			message[18]=c->state;
			message[19]=c->info[0];
			message[20]=c->info[1];
			break;
		case 4:
			printf("\nEnvoi data ADC  \n");
			message[17]=c->id;
			message[18]=c->state;
			message[19]=c->info[0];
			message[20]=c->info[1];
			break;
		default :
			printf("\nmessage dans le default\n");
			break;
		}message[taille+19]=0x00;
		message[taille+20]=checksum(message);
		int a;
		write(fd,message,taille+21);
		for(a=0;a<(taille+21);a++){printf("%02X ",message[a]);}
				printf("\n");
		free(message);
}

//fonction read côté FPGA

void readZ(int fd,capteur * liste){
int i=0;
for(i=0;i<7;i++){
int retour = -1;
unsigned char  rep[1];
int j = 20;
	while(retour ==-1 && j >= 0){
			retour = read(fd,rep,1);
				}
			j--;
							}
}
	printf("\n");


}



