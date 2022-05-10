#include <stddef.h>
#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <unistd.h>
#include <signal.h>
#include <sys/wait.h>

#define BUF_SIZE	30

#define handle_error(msg) \
    do { perror(msg); exit(EXIT_FAILURE); } while (0)

void sig_handler1(int num)
{
	printf("Pressed ctrl + c : %d\n",num);
	exit(EXIT_SUCCESS);
}

void wait_child(int num)
{
	printf("Wait for child pid\n");
	wait(NULL);
	exit(EXIT_SUCCESS);
}

int main(int argc, char const *argv[])
{
	if (signal(SIGINT, sig_handler1) == SIG_ERR) {
		handle_error("signal()");
		exit(EXIT_FAILURE);
	}

	pid_t child_pid;
	int fds[2];
	char msg[BUF_SIZE];

	if( pipe(fds) != 0 )
	{
		handle_error("pipe()");
	}

	child_pid = fork();

	if(0 < child_pid)
	{
		int write_len;
		if( close(fds[0]) != 0)
		{
			handle_error("close()");
		}
		while(1)
		{
			// Prevent zombie process 

            if(signal(SIGCHLD, wait_child) == SIG_ERR) {
				handle_error("signal()");
				exit(EXIT_FAILURE);
			}
			printf("\nThis is parent process, child_pid: %d, parent_pid: %d\n", child_pid, getpid());
			/* Nhập phản hồi từ bàn phím */
			printf("Please enter the message to send to child process: \n");
			fgets(msg, BUF_SIZE, stdin);
			write_len = write(fds[1],msg,BUF_SIZE);
			if( write_len == -1 )
			{
				handle_error("write()");
			}
			usleep(200);
		}

	} else if (0 == child_pid)
	{
		int read_len;
		
		if(close(fds[1]) != 0)
		{
			handle_error("close()");
		}
		while(1)
		{
			read_len = read(fds[0],msg,BUF_SIZE);
			if( read_len == -1 )
			{
				handle_error("read()");
			}
			printf("\nThis is child process, child_pid: %d, parent_pid: %d\n", getpid(), getppid());
			printf("Response from parent pid : %s\n", msg);
		}
	} else {
        handle_error("fork()");
    }
	return 0;

}

