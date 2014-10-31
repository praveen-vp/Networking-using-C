
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
#define BACKLOG 10

void sigchld_handler(int s) 
{
	while(wait(NULL) > 0);
}

int main()
{ 
	int sockfd, new_fd;
	struct sockaddr_in myaddr;
	struct sockaddr_in thaddr;
	int sin_size;
	struct sigaction sa;
	int yes=1;

	if((sockfd=socket(AF_INET, SOCK_STREAM, 0)) == -1) {
		perror("socket");
		exit(1);
	}

	if(setsockopt(sockfd, SOL_SOCKET, SO_REUSEADDR, &yes,sizeof(int)) == -1) {
		perror("setsocket");
		exit(1);
	}

	myaddr.sin_family = AF_INET;
	myaddr.sin_port = htons(PORT);
	myaddr.sin_addr.s_addr = INADDR_ANY;
	memset(&(myaddr.sin_zero),'\0',8);

	if(bind(sockfd, (struct sockaddr*)&myaddr, sizeof(struct sockaddr)) == -1) {
		perror("bind");
		exit(1);
	}

	if(listen(sockfd, BACKLOG) == -1) {
		perror("listen");
		exit(1);
	}

	sa.sa_handler = sigchld_handler;	 //reap all dead process
	sigemptyset(&sa.sa_mask);
	sa.sa_flags = SA_RESTART;
	if(sigaction(SIGCHLD, &sa, NULL) == -1) {
		perror("sigaction");
		exit(1);
	}

	//	mainloop
	while(1) {
		sin_size = sizeof(struct sockaddr_in);
		if((new_fd = accept(sockfd, (struct sockaddr *)&thaddr, &sin_size)) == -1) {
			perror("accept");
			exit(1);
		}

		printf("server: got connection from %s\n", inet_ntoa(thaddr.sin_addr));
		if(!fork()) { // this is child process
			close(sockfd);
			if(send(new_fd, "Hello, World\n", 14, 0) == -1)
				perror("send");
			close(new_fd);
			exit(0);
		}
		close(new_fd);
	}
	return 0;
}

