
#include <stdio.h>    /* Standard input/output definitions */
#include <stdlib.h>
#include "include/driver.h"
#include "include/socket.h"

int main(int argc, char *argv[])
{    (void)argc;
   

////////////////////////////////////////////*Définition des zigbee*/
////////////////////////////////////////////////////////////////////
    int fd = 0;
    capteur * liste=(capteur*)malloc(4*sizeof(capteur));

    zigbee z1={
        .adresse={0x00,0x13,0xa2,0x00,0x40,0x94,0x46,0xd0},
        .adresse_dynamique={0x00,0x00}
        };//le coordinateur


    zigbee z2={
        .adresse={0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00},
        .adresse_dynamique={0x77,0x4a}};
   

    (void)z1;
    (void)z2;
//on initialise la liste de 4 capteurs
    liste[0].id=1;
    liste[0].freq=0x00;
    liste[0].state=0;
    liste[0].info=(char*)malloc(2);
    liste[0].z=&z2;
    liste[1].id=2;
    liste[1].freq=0;
    liste[1].state=0;
    liste[1].info=(char*)malloc(3);
    liste[1].z=&z2;
    liste[2].id=3;
    liste[2].freq=0;
    liste[2].state=0;
    liste[2].info=(char*)malloc(2);
    liste[3].id=4;
    liste[3].freq=0;
    liste[3].state=0;
    liste[3].info=(char*)malloc(2);
    fd=serial_init("/dev/ttyUSB0", B9600);//serial_init(argv[2],atol(argv[2]));
   
   
////////////////////////////////////////////*Définition des sockets*/
////////////////////////////////////////////////////////////////////
     int listener;                        // connection listening socket
   
    struct pollfd fds[10];
    int nfds=0;
   
    char buf[MAX_MSG];                    // buffer for client data
    int nbytes;
   
    int timeout=250;                    // timeout in ms
   

    int flag=0;
    int quit=0;
   
    listener=tcp_server_socket(SERVER_PORT);
    if (listener<0) exit(1);
   
    printf("%s: waiting for connection on port TCP %u\n", argv[0], SERVER_PORT);

    socket_info(listener);
   
    /* initialze the fds to poll */
    memset(fds, 0, sizeof(fds));
    fds[1].fd = fd;
        fds[1].events = POLLIN;
    fds[0].fd = listener;
    fds[0].events = POLLIN;
    nfds+=2;
   


    while(!quit) {
        int rc=poll(fds, nfds, timeout);        // poll, wait forever
        if (rc < 0) {
            perror("  poll() failed");
            break;
        }
        if (rc == 0) {                    // timeout, will not happen here
           
            if(flag==1){
                const char *toSend=toXML(liste);
                for (int k=2;k<nfds;k++) write(fds[k].fd,toSend, strlen(toSend));
                flag=0;
            }
           
        }

       
   
        /* check for new connection */       
        if (fds[0].revents == POLLIN) {
            int client;            // client socket descriptor
            struct sockaddr_in clientaddr;        // client address
            socklen_t addrlen;        // used to store length (size) of sockaddr_in
        addrlen = sizeof(clientaddr);
        client = accept(listener, (struct sockaddr *)&clientaddr, &addrlen);
        if (client < 0) {
                perror("  accept() failed");
                break;
                }
         else {
                fprintf(stderr, "%s: new connection from %s, port %d on socket %d\n", argv[0], inet_ntoa(clientaddr.sin_addr), ntohs(clientaddr.sin_port), client);
                fds[nfds].fd = client;
                fds[nfds].events = POLLIN;
                nfds++;
               
            }
            rc--;
        }
       
        if(fds[1].revents ==POLLIN){       
                    readZ(fd,liste);
                    flag=1;
           
                    if(liste[0].state==0x11){
                        printf("la température vaut %02X, %02X degrées celsius\n",liste[0].info[0],liste[0].info[1]);}
                    if(liste[1].state==0x11){
                        printf("le gyroscope indique  %02X en x, %02X en y %02X en z\n",liste[1].info[0],liste[1].info[1],liste[1].info[2]);}
                    if(liste[2].state==0x11){
                        printf("le luxmetre indique  %02X ,%02X lumens\n",liste[2].info[0],liste[2].info[1]);}
                    if(liste[3].state==0x11){
                        printf("le potar indique  %02X ,%02X \n",liste[3].info[0],liste[3].info[1]);}
                        }
                        


       /* check for data from a connected client */
        int i=2;
        while (i<nfds) {
            if (!rc) break;
            if (fds[i].revents == POLLIN) {
                nbytes=read(fds[i].fd, buf, sizeof(buf));
                if (nbytes < 0) {
                    perror("read client socket failed!");
                } else if (nbytes == 0) {     // connection closed!
                    fprintf(stderr, "%s: connection on socket %d closed\n", argv[0], fds[i].fd);
                    close(fds[i].fd);
                    fds[i].fd = -1;
                    /* compress the fds array */
                    for (int j=i ; j<nfds-1 ; j++) fds[j].fd = fds[j+1].fd;
                    nfds--;
                    i--;
                } else {
                    if(*buf=='1' || *buf=='0'){
                        if(liste[0].freq==0x00){liste[0].freq=0x01;}
                        else liste[0].freq=0x00;
                        writeZ(&liste[0],fd);
                    }
                    else if(*buf=='2'){
                        system("cam_server2 -f UYVY -s 640x480 --capture=1000 --stdout /dev/video6 | fbcamviewer -p3 -w640*480 &");}
                    //else on lance le programme de jo
                   
                }
                rc--;
            }
            i++;
        }
    }
  //  close(fd);
    close(listener);
    exit(0);


 
}

