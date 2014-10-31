
// compile the pgm to server and client and run
// server and client on differnt terminls 

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <errno.h>
#include <netdb.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>

#define DPORT 4000
#define RPORT 4000
#define MAXBUFLEN 10000

int my_send(struct hostent *he,int port);
int my_recv(int port);

int main(int argc, char *argv[])
{
	struct hostent *he;
	fd_set readfds;
	struct timeval tv;

	tv.tv_sec = 5;
	tv.tv_usec = 0;
 
	FD_ZERO(&readfds);
	FD_SET(0, &readfds);
	FD_SET(RPORT, &readfds);
	FD_SET(DPORT, &readfds);

	if(argc != 2) {
		fprintf(stderr, "usage address");
		exit(1);
	}

	if((he=gethostbyname(argv[1])) == NULL ) {
		perror("gethostbyname ");
		exit(1);
	}

	if( strncmp(argv[0],"./server",3) == 0)
		goto server;
	
	while(1) {
		
		printf("your msg===>>>\n");

		if (my_send(he,DPORT) == 5)
			printf("==>>msg send!\n");
		else 
			printf("!!! sending failed.\n");
		my_recv(RPORT);
	}	

server:	while(1) {

		my_recv(DPORT);
			
		printf("your msg===>>>\n");
		
		if (my_send(he,RPORT) == 5)
			printf("==>>msg send!\n");
		else 
			printf("!!! sending failed.\n");
	}

	return 0;
}

int my_send(struct hostent *he,int port) {
	
	int sockfd;
	struct sockaddr_in their_addr;
	int numbytes;
	char msg[25];
		
	if((sockfd = socket(AF_INET,SOCK_DGRAM,0)) == -1) {
			perror("socket");
			exit(1);
	}

	their_addr.sin_family = AF_INET;
	their_addr.sin_port = htons(port);
	their_addr.sin_addr = *((struct in_addr *)he->h_addr);

	memset(&(their_addr.sin_zero),'\0',8);

	scanf("%s",msg);

	if((numbytes = sendto(sockfd,msg, strlen(msg), 0, 
				(struct sockaddr *)&their_addr, 
					sizeof(struct sockaddr))) == -1) {
		perror("sendto");
		exit(1);
	}
	close(sockfd);
	
	return 5;
}

int my_recv(int port)
{
	int sockfd;
	struct sockaddr_in my_addr;
	struct sockaddr_in their_addr;
	int addr_len, numbytes;
	char buf[MAXBUFLEN];

	if((sockfd = socket(AF_INET,SOCK_DGRAM,0)) == -1) {
		perror("socket");
		exit(1);
	}

	my_addr.sin_family = AF_INET;
	my_addr.sin_port = htons(port);
	my_addr.sin_addr.s_addr = INADDR_ANY;
	memset(&(my_addr.sin_zero),'\0',8);

	if(bind(sockfd,(struct sockaddr *)&my_addr, 
				sizeof(struct sockaddr)) == -1) {
		perror("bind");
		exit(1);
	}
		
	addr_len = sizeof(struct sockaddr);

	if((numbytes = recvfrom(sockfd,buf,MAXBUFLEN-1,0,
				(struct sockaddr *)&their_addr, &addr_len )) == 1 && (errno != 0)) {
		perror("recieve from:");
		exit(1);
	}

	printf("msg from %s: ==>> ",inet_ntoa(their_addr.sin_addr));
	buf[numbytes] = '\0';
	printf("%s \n",buf);
	close(sockfd);
	return 5;
}

