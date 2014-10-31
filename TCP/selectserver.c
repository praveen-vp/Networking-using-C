// select server

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>

#define PORT 4000
int main()
{
	fd_set master; 		//master file descripter list
	fd_set read_fds;
	struct sockaddr_in myaddr;
	struct sockaddr_in remoteaddr;

	int fdmax, listener, newfd, nbytes, addrlen, i, j;
	int yes = 1;
	char buf[256];

	FD_ZERO(&master);
	FD_ZERO(&read_fds);

	// get the listener
	if((listener = socket(AF_INET, SOCK_STREAM, 0) == -1)) {
		perror("socket");
		exit(1);
	}

	// lose the pesky "address already in use" error message
	if(setsockopt(listener, SOL_SOCKET, SO_REUSEADDR,
				&yes, sizeof(int)) == -1) {
		perror("setsockopt");
		exit(1);
	}

	// bind
	myaddr.sin_family = AF_INET;
	myaddr.sin_addr.s_addr = INADDR_ANY;
	myaddr.sin_port = htons(PORT);
	memset(&(myaddr.sin_zero), '\0', 8);
	if(bind(listener, (struct sockaddr *)&myaddr, sizeof(myaddr)) == -1) {
		perror("bind");
		exit(1);
	}

	// listen
	if(listen(listener, 10) == 1) {
		perror("listener");
		exit(1);
	}

	// add the listener to the master set 
	FD_SET(listener, &master);

	// keep track of the biggest file descripter
	fdmax = listener; // so far, it's this one

	// main loop
	for(;;) {
		read_fds = master;
		if(select(fdmax+1, &read_fds, NULL, NULL, NULL) == -1) {
			perror("select");
			exit(1);
		}

		// run through the existing connections looking for data to read
		for(i = 0; i <= fdmax; i++) {
			if(FD_ISSET(i, &read_fds)) { // we get one
				if(i == listener) {
					// handle new connections
					addrlen = sizeof(remoteaddr);
					if((newfd = accept(listener, (struct sockaddr *)&remoteaddr, &addrlen)) == -1)
						perror("accept");
					else {
						FD_SET(newfd, &master);
						if(newfd > fdmax)  // keep track of maximum
							fdmax = newfd;
					printf("selectserver: new connection from %s on"
							"socket %d\n", inet_ntoa(remoteaddr.sin_addr), newfd);
					}
				} else {
					// handle data from a client
					if((nbytes = recv(i, buf, sizeof(buf), 0)) <= 0) {
						// got error on connection closed by client
						if(nbytes == 0) 
							// connection closed
							printf("selectserver: socket %d hung up\n",i);
						else 
							perror("recv");
					
						close(i); // bye
						FD_CLR(i, &master);	// remove from master set
					} else {
						// we got some data from a client
						for(j = 0; j <= fdmax; j++) {
							// send to every one!
							if(FD_ISSET(j, &master)) {
								// except the listener and ourselves
								if(j != listener && j != i) {
									if(send(j, buf, nbytes, 0) == -1)
										perror("send");
								}
							}
						}
					}
				}
			}
		}
	}
	return 0;
}

