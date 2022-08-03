#define _GNU_SOURCE
#include <errno.h>
#include <fcntl.h>
#include <sched.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/prctl.h>
#include <sys/stat.h>
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

int main(int argc, char * const argv[]) {
	char *args[2] = {"main2", NULL};

	prctl(PR_SET_CHILD_SUBREAPER, 1, 0, 0, 0);
	

	char ns_path[100];
	sprintf(ns_path, "/proc/%s/ns/pid", argv[1]);
	int fd = open(ns_path, O_RDONLY | O_CLOEXEC);
	if (fd == -1) {
		perror("open: ");
		exit(1);
	}
	if (setns(fd, 0) == -1) {
		perror("setns: ");
		exit(1);
	}

	int grandchild_pid = fork();

	int stdout_fd;

	if ( grandchild_pid == 0 ) {
		stdout_fd = open("/run/qianminj_exp/output2", O_WRONLY | O_CREAT, 0666);
		dup2(stdout_fd, 1);
		close(stdout_fd);

	//	sleep (200);
		printf( "This is being printed from the child process\n" );
		execv(args[0], args);
		printf("execv failed with errno %d", errno);	
	} else {
		int p;
		prctl(PR_GET_CHILD_SUBREAPER, &p);
		printf( "This is being printed in the parent process:\n"
		        " - the process identifier (pid) of the child is %d, the subreaper flag is %d\n", grandchild_pid, p );
		int wstatus;
		wait(&wstatus);
		sleep(1000);
	}
	close(fd);

	return 0;
}
