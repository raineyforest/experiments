#define _GNU_SOURCE
#include <errno.h>
#include <fcntl.h>
#include <sched.h>
#include <stdio.h>
#include <stdlib.h>
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
	char *args[3] = {"/usr/bin/cat", "/run/qianminj_exp/output1", NULL};
	// char *args[3] = {"/usr/bin/sleep", "100", NULL};

	int fd0 = mkdir("/run/qianminj_exp", 0777);
	if (fd0 == -1) {
		perror("mkdir: ");
		exit(1);
	}
	fd0 = open("/run/qianminj_exp/output1", O_CREAT | O_WRONLY | O_APPEND, 0777);
	if (fd0 == -1) {
		perror("open output1:");
		exit(1);
	}
	char text[50] = "qianminjdebugprintsomething";
	ssize_t cnt = write(fd0, text, 40);
	printf("wrote %d chars into the file", cnt);	
	close(fd0);

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

	int pid = fork();

	int stdout_fd;

	if ( pid == 0 ) {
		stdout_fd = open("/run/qianminj_exp/output2", O_WRONLY | O_CREAT, 0666);
		dup2(stdout_fd, 1);
		close(stdout_fd);

		sleep (20);
		printf( "This is being printed from the child process\n" );
		execv(args[0], args);
		printf("execv failed with errno %d", errno);	
	} else {
		printf( "This is being printed in the parent process:\n"
		        " - the process identifier (pid) of the child is %d\n", pid );
	}
	close(fd);

	return 0;
}
