#ifndef SOCKET_H
#define SOCKET_H
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>

#include <stdio.h>
#include <unistd.h> 
#include <string.h>
#include <signal.h>
#include <stdlib.h>
#include <fcntl.h>

#include <sys/poll.h>
#include <bits/poll.h>


#define SERVER_PORT 2222

#define MAX_MSG 256

#define LED_NAME	"/sys/class/leds/ds7/brightness"


int socket_info(int sock);
int tcp_server_socket(int port);
void get_data(int d[]);
#endif
