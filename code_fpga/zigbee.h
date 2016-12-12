
#ifndef ZIGBEE_H_
#define ZIGBEE_H_

#include "terasic_includes.h"

#include <stdint.h> /* Standard types */



#include <errno.h> /* Error number definitions */
#include "sys/termios.h" /* POSIX terminal control definitions */
#include <sys/ioctl.h>
#include <getopt.h>

typedef struct zigbee{
unsigned char adresse[8];
unsigned char adresse_dynamique[2];
} zigbee;

typedef struct capteur{
char id;
char freq;
char * info;
char state;
zigbee * z;
} capteur;

void refreshState(unsigned char *,capteur * liste);
int serial_init(void);
unsigned char checksum(unsigned char * trame);
void writeZ(capteur * c,int fd);
void readZ(int fd,capteur * liste);

#endif /* ZIGBEE_H_ */


