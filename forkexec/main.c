#include <errno.h>
#include <stdio.h>
#include <sys/wait.h>
#include <unistd.h>

/* This program forks and and the prints whether the process is
 *   - the child (the return value of fork() is 0), or
 *   - the parent (the return value of fork() is not zero)
 *
 * When this was run 100 times on the computer the author is
 * on, only twice did the parent process execute before the
 * child process executed.
 *
 * Note, if you juxtapose two strings, the compiler automatically
 * concatenates the two, e.g., "Hello " "world!"
 */

int main( void ) {
	char *args[3] = {"/usr/bin/sleep", "50", NULL};

	int pid = fork();

	if ( pid == 0 ) {
		printf( "This is being printed from the child process\n" );
		execv(args[0], args);
		printf("execv failed with errno %d", errno);	
	} else {
		printf( "This is being printed in the parent process:\n"
		        " - the process identifier (pid) of the child is %d\n", pid );
	}
	int wstatus;
	wait(&wstatus);

	return 0;
}
