
#include <stdio.h>
#include <sys/wait.h>
#include <unistd.h>

int main (void) {
 sleep(2);
 char* args[3] = {"/usr/bin/sleep", "500", NULL};
 int pid = fork();
 if (pid == 0) {
 	execv(args[0], args);
 } else {
 	int wstatus;
	wait(&wstatus);
 }
 return -1;
}
