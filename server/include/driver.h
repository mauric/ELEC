#ifndef DRIVER_H
#define DRIVER_H

#include <stdio.h>    /* Standard input/output definitions */
#include <stdlib.h> 
#include <stdint.h>   /* Standard types */
#include <string.h>   /* String function definitions */
#include <unistd.h>   /* UNIX standard function definitions */
#include <fcntl.h>    /* File control definitions */
#include <errno.h>    /* Error number definitions */
#include <termios.h>  /* POSIX terminal control definitions */
#include <sys/ioctl.h>
#include <getopt.h>
#include <math.h>
typedef struct zigbee{
	unsigned char adresse[8];	
	unsigned char adresse_dynamique[2];
	} zigbee;

typedef struct capteur{
	char id;
	char freq;
	char state;
	char * info;
	zigbee * z;
	} capteur;

void refreshState(unsigned char *,capteur * liste);
int serial_init(const char *devname, speed_t baudrate);
unsigned char checksum(unsigned char * trame);
void writeZ(capteur * c,int fd);
void readZ(int fd,capteur * liste);
void get_temp(char * rep,capteur * liste);
void get_lux(char * rep,capteur * liste);
void get_acc(char * rep,capteur * liste);
void get_can(char * rep,capteur * liste);
const char * toXML(capteur * liste);
#endif
