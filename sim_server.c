
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

#define MYPORT 4000
#define MAXBUFLEN 10000

int main()
{
	int sockfd;
	struct sockaddr_in my_addr;
	struct sockaddr_in their_addr;
	int addr_len, numbytes;
	char buf[MAXBUFLEN];

	while(1) {

		if((sockfd = socket(AF_INET,SOCK_DGRAM,0)) == -1) {
			perror("socket");
			exit(1);
		}

		my_addr.sin_family = AF_INET;
		my_addr.sin_port = htons(MYPORT);
		my_addr.sin_addr.s_addr = INADDR_ANY;
		memset(&(my_addr.sin_zero),'\0',8);


		if(bind(sockfd,(struct sockaddr *)&my_addr, 
					sizeof(struct sockaddr)) == -1) {
			perror("bind");
			exit(1);
		}
		
		addr_len = sizeof(struct sockaddr);

		if((numbytes = recvfrom(sockfd,buf,MAXBUFLEN-1,0,
				(struct sockaddr *)&their_addr, &addr_len )) == 1) {
			perror("recieve from:");
			exit(1);
		}

		printf("msg from %s: ==>> ",inet_ntoa(their_addr.sin_addr));
		buf[numbytes] = '\0';
		printf("%s \n",buf);
	
		close(sockfd);
	}
	
	return 0;
}

