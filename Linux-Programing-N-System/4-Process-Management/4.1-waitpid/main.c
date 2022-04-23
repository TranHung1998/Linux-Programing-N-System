#include <stdio.h>
#include <unistd.h>
#include <sys/wait.h>
#include <sys/types.h>

void main()
{
	pid_t childPid, pid;
	childPid = fork();
	int status;
	switch(childPid)
	{
		case -1:
			//Error
			printf("Error fork() create child process\n");
			break;
		case 0:
			printf("This is child process\n");

			printf("Child PID: %d\n", getpid());
			sleep(5);
			break;
		default:
			printf("This is father process\n");
			printf ("childpid=%d\n" , childPid);
			printf("Parent PID: %d\n", getpid());
			pid = waitpid(childPid, &status, NULL);
			printf("End of child process\n");
			if( -1 == pid)
			{
				printf("Error, wait\n" );
			}
			printf ("pid=%d\n" , pid);

			if (WIFEXITED (status))
				printf ("Normal termination with exit status=%d\n", WEXITSTATUS (status));
			if (WIFSIGNALED (status))
				printf ("Killed by signal=%d%s\n" ,WTERMSIG (status), WCOREDUMP (status) ? " (dumped core)" : "" );
			if (WIFSTOPPED (status))
				printf ("Stopped by signal=%d\n", WSTOPSIG (status));
			if (WIFCONTINUED (status))
				printf ("Continued\n" );
			break;
				
}	
	
}
