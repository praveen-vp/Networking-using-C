
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

#define MYPORT 4950
#define MAXBUFLEN 100

int main(int argc, char *argv[])
{
	int sockfd;
	struct sockaddr_in their_addr;
	struct hostent *he;
	int numbytes;

	if(argc != 3) {
		fprintf(stderr, "usage: client hostname message");
		exit(1);
	}

	if((he=gethostbyname(argv[1])) == NULL ) {
		perror("gethostbyname:");
		exit(1);
	}

	if((sockfd = socket(AF_INET,SOCK_DGRAM,0)) == -1) {
		perror("socket");
		exit(1);
	}

	their_addr.sin_family = AF_INET;
	their_addr.sin_port = htons(MYPORT);
	their_addr.sin_addr = *((struct in_addr *)he->h_addr);

	memset(&(their_addr.sin_zero),'\0',8);

	if((numbytes = sendto(sockfd, argv[2], strlen(argv[2]), 0, 
					(struct sockaddr *)&their_addr, sizeof(struct sockaddr))) == -1) {
		perror("sendto:");
		exit(1);
	}

	printf("sent %d bytes to %s\n",numbytes,inet_ntoa(their_addr.sin_addr));
	close(sockfd);

	return 0;
}

