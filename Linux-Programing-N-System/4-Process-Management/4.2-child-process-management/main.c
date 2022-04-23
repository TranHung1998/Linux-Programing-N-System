#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>
#include <sys/types.h>
#include <fcntl.h>
#include <string.h>


void B2_process()
{
	int fd;
	char buf[20] = "";
	sprintf(buf,"Hi I'm B' process\n");
	fd = open("./file.txt", O_WRONLY);
	lseek(fd, 0, SEEK_END);
	write(fd, buf, (int)strlen(buf));
	close(fd);	
}

void B_process()
{
	int fd;
	char buf[20] = "";
	sprintf(buf,"Hi I'm B process\n");
	fd = open("./file.txt", O_WRONLY);
	lseek(fd, 0, SEEK_END);
	write(fd, buf, (int)strlen(buf));
	close(fd);
}

void C2_process()
{
	int fd;
	char buf[20] = "";
	sprintf(buf,"Hi I'm C' process\n");
	fd = open("./file.txt", O_WRONLY);
	lseek(fd, 0, SEEK_END);
	write(fd, buf, (int)strlen(buf));
	close(fd);
}

void C_process()
{
	int fd;
	char buf[20] = "";
	sprintf(buf,"Hi I'm C process\n");
	fd = open("./file.txt", O_WRONLY);
	lseek(fd, 0, SEEK_END);
	write(fd, buf, (int)strlen(buf));
	close(fd);
}

void main()
{
	pid_t child[2], grand[2];
        pid_t pid;
	int status;

	char buf[20] = "";
	
	int fd;	
	fd = open("file.txt" ,O_RDWR | O_CREAT, 0667);
	close(fd);

	for(int i = 0; i < 2 ; i++)
	{
		child[i] = fork();

		if(0 == child[i])
		{
			grand[i] = fork();
			if(0 == grand[i])
			{
				if( 0 == i)
					B2_process();
				if( 1 == i)
					C2_process();
				exit(0);
			}
			wait(&status);
			if( 0 == i)
				B_process();
			if( 1 == i)
				C_process();
			exit(0);
		}
	}

	if(0 < child[0] && 0 < child[1])
	{
		pid = waitpid(child[0], NULL, NULL);
		sprintf(buf,"Hi I'm A process\n");
		fd = open("./file.txt", O_WRONLY);
		lseek(fd, 0, SEEK_END);
		write(fd, buf, (int)strlen(buf));
		close(fd);
		pid = waitpid(child[1], &status, NULL);
	}

}
