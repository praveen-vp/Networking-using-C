
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
#include <fcntl.h>
#include <sys/stat.h>

#define PORT 4000
#define BACKLOG 10
#define MAXSIZE 1000000

int file_size(char* fname);
char* extn(char* fname);

void sigchld_handler(int s) 
{
	while(wait(NULL) > 0);
}

int main(int argc, char ** argv)
{ 
	int sockfd, new_fd;
	struct sockaddr_in myaddr;
	struct sockaddr_in thaddr;
	int sin_size, numbytes;
	struct sigaction sa;
	int yes=1;
	int fd;
	char buff[MAXSIZE];
	char header[MAXSIZE] ;

	chdir("/home/praveen");//Working-Directory/Networking/TCP");
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

//		printf("server: got connection from %s\n%s", inet_ntoa(thaddr.sin_addr),buff);
		if(!fork()) { // this is child process
			close(sockfd);
			
			if((numbytes=recv(new_fd, buff, MAXSIZE-1, 0)) == -1) {
				perror("recv");
				exit(1);
			}

			printf("%s\n",buff);
			char get[12],fname[12];
			sscanf(buff, "%s %s",get, fname ); //get file name 
			printf("%s ==>> %s\n", get, fname);

			if((fd=open(fname+1, O_RDONLY)) < 0 ) {
				perror("open");
				exit(1);
			}
			if((read(fd, buff, file_size(fname+1))) < 0) {
				perror("read");
				exit(1);
			}

//			printf("type =>%d %s",file_size(fname+1),extn(fname+1));
			sprintf(header,"HTTP/1.1 200 OK\nContent-Length:%d\nConnection:close\nContent-Type:\
				   %s\n\n",file_size(fname+1),extn(fname+1));
			bcopy(buff,header+strlen(header),file_size(fname+1)); // concatinate header & buff 
	
			printf("%s\n\n",header);
			if(send(new_fd, header, sizeof(header), 0) == -1)
				perror("send");
	
			close(new_fd);
			exit(0);
		}
		close(new_fd);
	}
	return 0;
}

char* extn(char* fname) 
{
	char *p = strrchr(fname, '.');
	if(strcmp(p,".jpg") == 0)
		return "image/jpg";
	else if(strcmp(p,".png") == 0)
		return "image/png";
	else if(strcmp(p,".gif") == 0)
		return "image/gif";
	else if(strcmp(p,".html") == 0)
		return "text/html";
	else return "text/txt";
}

int file_size(char* fname) 
{
	struct stat size;
	stat(fname, &size);
	return size.st_size;
}

