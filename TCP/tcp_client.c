
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <sys/types.h>
#include <errno.h>
#include <netdb.h>
#include <unistd.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <signal.h>
#include <sys/wait.h>

#define PORT 4000
#define MAXSIZE 100

int main(int argc, char* argv[]) 
{
	int sockfd, numbytes;
	char buf[MAXSIZE];
	struct hostent *he;
	struct sockaddr_in thaddr;
	char sendbuf[25] = "Hai!";

	if(argc != 2) {
		fprintf(stderr, "usage: client hostname\n");
		exit(1);
	}

	if((he=gethostbyname(argv[1])) == NULL) {
		perror("gethostbyname");
		exit(1);
	}

	if((sockfd=socket(AF_INET, SOCK_STREAM, 0)) == -1) {
		perror("sockfd");
		exit(1);
	}
	thaddr.sin_family = AF_INET;
	thaddr.sin_port = htons(PORT);
	thaddr.sin_addr = *((struct in_addr*)he->h_addr);
	memset(&(thaddr.sin_zero), '\0', 8);

	if(connect(sockfd, (struct sockaddr *)&thaddr, sizeof(struct sockaddr)) == -1) {
		perror("connect");
		exit(1);
	}
	fgets(sendbuf, MAXSIZE, stdin);

	if(send(sockfd, sendbuf, 14, 0) == -1) {
		perror("send");
		exit(1);
	}

	if((numbytes=recv(sockfd, buf, MAXSIZE-1, 0)) == -1) {
		perror("recv");
		exit(1);
	}
	buf[numbytes] = '\0';
	printf("Received: %s\n",buf);
	close(sockfd);
	return 0;
}

