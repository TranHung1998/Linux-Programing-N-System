#include <stdio.h>
#include <unistd.h>
#include <sys/wait.h>
#include <sys/types.h>
#include <stdlib.h>

void wait_child()
{
	printf("wait_child \n");
	wait(NULL);
}
void main()
{
	signal(SIGCHLD,wait_child);
	pid_t childPid[5000];
	pid_t pid;
	for(int i = 0; i < 5000 ; i++)
	{
		childPid[i] = fork();
		if(0 == childPid[i])
			exit(NULL);
	}
}	
	
