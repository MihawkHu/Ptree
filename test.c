#include <unistd.h>
#include <sys/syscall.h>
#include <sys/types.h>
#include <stdio.h>
#include <stdlib.h>

int main(int argc, char *argv[]) {
	pid_t parent_pid = getpid();	//get current process pid
	pid_t child_pid = fork();	//get child process pid
	
	if (child_pid == 0) {
		printf("5140519019 Parent, %d\n", parent_pid);	
		printf("5140519019 Child, %d\n", child_pid);	
		execl("./ptree", "ptree", (char*)0);	// execute ptree in child process	
	}
	return 0;
}
