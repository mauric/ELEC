#include "../include/socket.h"




/***************************************************************************
 * socket utils
 ***************************************************************************/
int socket_info(int sock)
{
	struct sockaddr_in	addr;
	socklen_t			len;
	
	len = sizeof(struct sockaddr_in);
	if (getsockname(sock, (struct sockaddr*)&addr, &len) < 0) {
		perror("getsockname");
		return -1;
	}
	fprintf(stdout, "Server IP: %s, port: %u\n", inet_ntoa(addr.sin_addr), ntohs(addr.sin_port));
	
	return 0;
}

int tcp_server_socket(int port)
{
	int sock, on=1;
	struct sockaddr_in serv_addr;  /* socket address for server */
	
    /* now create the server socket 
       make it an IPV4 socket (PF_INET) and stream socket (TCP)
       and 0 to select default protocol type */          
    sock = socket(AF_INET, SOCK_STREAM, 0);
    if (sock < 0) {
        perror("  cannot open socket");
        return -1;
    }
  
  	/* allow socket descriptor to be reusable */
  	if (setsockopt(sock, SOL_SOCKET, SO_REUSEADDR, &on, sizeof(on)) < 0) {
  		perror("  setsockopt() failed");
  		close(sock);
  		return -1;
  	}
  	
    /* now fill in values of the server sockaddr_in struct 
       s_addr and sin_port are in Network Byte Order (Big Endian)
       Byte order conversion may be necessary (e.g. htons(), and htonl() */    
	memset(&serv_addr, 0, sizeof(struct sockaddr_in));
    serv_addr.sin_family = AF_INET;  /* again ipv4 */  
    serv_addr.sin_addr.s_addr = htonl(INADDR_ANY); /* local address */
    serv_addr.sin_port = htons(port); 
        
    /* now bind server port 
       associate server socket (sock) with IP address:port (serv_addr) */ 
    if (bind(sock, (struct sockaddr *)&serv_addr, sizeof(struct sockaddr)) < 0) {
        perror("  bind() failed");
        close(sock);
        return -1;
    }
    
    /* wait for connection from client with a pending queue of size 5 */
    if (listen(sock, 5) < 0) {
    	perror("  listen() failed");
    	close(sock);
    	return -1;
    }
    
	return sock;
}


void get_data(int d[])
{
	// read /proc/stat
    char bf[256];
	int fd=open("/proc/stat", O_RDONLY);
	int n=read(fd,bf,100);
	bf[n]=0;
	close(fd);
			
	char*p=bf;
	char tmp[24];
	while (*p!=' ') p++;p++;			// skip first word

	int i=0;
	for (int k=0;k<4;k++) {
		do {
			tmp[i++]=*p++;
		} while (*p!=' ');
		tmp[i]=0;
		d[k]=atoi(tmp);
		p++;
		i=0;
	}
}

