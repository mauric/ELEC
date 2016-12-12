/*
 * main.c
 *
 *  Created on: 27 avr. 2016
 *      Author: a3quirat
 */
#include <fcntl.h>

#include "sys/alt_irq.h"
#include "sys/ioctl.h"
#include "sys/alt_errno.h"

#include "altera_avalon_uart.h"
#include "altera_avalon_uart_regs.h"
#include <stdio.h>
#include "I2C.h"
#include "capteurs.h"
#include "terasic_includes.h"
#include "stdlib.h"
#include "accelerometer_adxl345.h"
#include <stdio.h>
#include "zigbee.h"


int main()
{

char temper[2];
char lum[2];
char acc[3];



int fd = 0;
capteur * tableau=(capteur*)malloc(4*sizeof(capteur));
zigbee zb={
.adresse={0x00,0x13,0xa2,0x00,0x40,0x94,0x46,0xd2}//00 13 A2 00 40 89 EB FA//00 13 A2 00 40 89 EC 32///00 13 A2 00 40 94 46 D2
};//adresse  coordinateur
//on initialise la tableau de 4 capteurs
tableau[0].id=1;
tableau[0].state=0x11;
tableau[0].info=(char*)malloc(2);
tableau[0].z=&zb;
tableau[0].freq=0;
tableau[1].id=2;
tableau[1].state=0x11;
tableau[1].info=(char*)malloc(3);
tableau[1].z=&zb;
tableau[2].id=3;
tableau[2].state=0x11;
tableau[2].info=(char*)malloc(2);
tableau[2].z=&zb;
tableau[3].id=4;
tableau[3].state=0x11;
tableau[3].info=(char*)malloc(2);
tableau[3].z=&zb;
fd=serial_init();
while(1){


//On lit la température + envoit !
temp(temper);
tableau[0].info[0]=temper[0];
writeZ(&tableau[0],fd);
readZ(fd,tableau);

//la lumiére
light(lum);
tableau[2].info[0]=lum[0];
tableau[2].info[1]=lum[1];
writeZ(&tableau[2],fd);
readZ(fd,tableau);

//accell
accel(acc);
tableau[1].info[0]=acc[0];
tableau[1].info[1]=acc[1];
tableau[1].info[2]=acc[2];
writeZ(&tableau[1],fd);
readZ(fd,tableau);
//convertisseur

ADC_0(SS, IN0, &DataADC);
		Data[1] = DataADC;
		Data[0] = DataADC >> 8;
		tableau[3].info[0]=Data[0];
		tableau[3].info[1]=Data[1];
		usleep(300000);
	    writeZ(&tableau[3],fd);
		readZ(fd,tableau);




}
return 0;
}



