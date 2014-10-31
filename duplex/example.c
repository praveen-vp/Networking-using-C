
#include <stdio.h>
#include <stdlib.h>
#include <sys/time.h>
#include <sys/types.h>
#include <unistd.h>

int main()
{
	fd_set rdfs;
	struct timeval tv;
	int retval;

	// watch stdin (fd 0) to see whereit has input 
	FD_ZERO(&rdfs);
	FD_SET(0, &rdfs);

	// wait up to 5 sec
	tv.tv_sec = 5;
	tv.tv_usec = 0;

	retval = select(1, &rdfs, NULL, NULL, &tv);
	// don't relay on the value of tv now

	if(retval == -1)
		perror("select()");
	else if(retval)
		printf("Data is avialable now. >>> %d\n",retval);
		// FD_ISSET(0, &rdfs) will be true
	else 
		printf("No data within five seconds\n");

	exit(EXIT_SUCCESS);
}

