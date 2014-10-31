
// client 
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

#define DPORT 4950
#define RPORT 3000
#define MAXBUFLEN 1000

int main(int argc, char* argv[])
{
	struct hostent *he;
	int addr_len;
	int sockfd,numbytes;
	char msg[255];
	struct sockaddr_in their_addr;

	if(argc != 2) {
		fprintf(stderr, "usage: client hostname\n");
		exit(1);
	}
	if((he=gethostbyname(argv[1])) == NULL ) {
		perror("gethostbyname");
		exit(1);
	}
	their_addr.sin_family = AF_INET;
	their_addr.sin_port = htons(DPORT);
	their_addr.sin_addr = *((struct in_addr *)he->h_addr);
	memset(&(their_addr.sin_zero),'\0',8);

	if((sockfd = socket(AF_INET, SOCK_DGRAM, 0)) == 0 ) {
		perror("socket");
		exit(1);
	}

	while(1) {

		printf("your msg\n");
		gets(msg);
		if((numbytes = sendto(sockfd,msg, strlen(msg), 0, 
					(struct sockaddr *)&their_addr, 
					sizeof(struct sockaddr))) == -1) {
			perror("sendto");
			exit(1);
		}
		addr_len = sizeof(struct sockaddr);
	
		if((numbytes = recvfrom(sockfd, msg, MAXBUFLEN-1, 0,
					(struct sockaddr *)&their_addr,
					&addr_len)) == 1  && errno != 0) {
			perror("recv");
			exit(1);
		}
		printf("msg from %s:=>>", inet_ntoa(their_addr.sin_addr));
		msg[numbytes] = '\0';
		printf("[ %s ]\n",msg);
	}
	close(sockfd);

	return 0;
}

