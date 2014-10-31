
#include <stdio.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <unistd.h>
#include <stdlib.h>
#include <errno.h>

int main()
{
	int s1,s2,n,rv;
	fd_set readfds;
	char buf1[256], buf2[256];
	struct timeval tv;

	FD_ZERO(&readfds);
	FD_SET(s1, &readfds);
	FD_SET(s2, &readfds);

	n = s2+1;
	tv.tv_sec = 10;
	tv.tv_usec = 500000;
	rv = select(n, &readfds, NULL, NULL, &tv);

	if(rv == -1)
		perror("select");
	else if(rv == 0) 
		printf("Timeout occured! No data after 10.5 s.\n");
	else {
		// one or both of the descriptors have data 
		if(FD_ISSET(s1, &readfds))
			recv(s1, buf1, sizeof(buf1), 0);
		if(FD_ISSET(s2, &readfds))
			recv(s1, buf2, sizeof(buf2), 0);
	}
}

