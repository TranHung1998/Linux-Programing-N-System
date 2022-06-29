#include <stddef.h>
#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/stat.h>
#include <signal.h>
#include <sys/wait.h>

#define BUF_SIZE	30
#define FIFO_PATH	"./fifo_file"

#define handle_error(msg) \
    do { perror(msg); exit(EXIT_FAILURE); } while (0)

#define handle_error_no_exit(msg) (perror(msg))

void sig_handler1(int num)
{
	printf("Pressed ctrl + c : %d\n",num);
	unlink(FIFO_PATH);
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
	char msg[BUF_SIZE];
	int fd;

	if(mkfifo(FIFO_PATH, 0666) != 0)
	{
		handle_error_no_exit("mkfifo()");
	}

	child_pid = fork();

	if(0 < child_pid)
	{
		// Prevent zombie process 
		if(signal(SIGCHLD, wait_child) == SIG_ERR) {
			handle_error("signal()");
			exit(EXIT_FAILURE);
		}
		int write_len;
		fd = open(FIFO_PATH, O_WRONLY);
		while(1)
		{
			printf("\nThis is parent process\n");
			//printf("Child_pid: %d, parent_pid: %d\n", child_pid, getpid());
			/* Nhập phản hồi từ bàn phím */
			printf("Please enter the message to send to child process: \n");
			fgets(msg, BUF_SIZE, stdin);
			
			if( fd == -1)
			{
				handle_error("open()");
			}
			write_len = write(fd,msg,BUF_SIZE);
			if( write_len == -1 )
			{
				handle_error("write()");
			}
			//close(fd);
			usleep(200);
		}
	} else if (0 == child_pid)
	{
		int read_len;
		fd = open(FIFO_PATH, O_RDONLY);
		while(1)
		{
			if( fd == -1)
			{
				handle_error("open()");
			}
			read_len = read(fd,msg,BUF_SIZE);
			if( read_len == -1 )
			{
				handle_error("read()");
			}
			//close(fd);
			printf("\nThis is child process\n");
			//printf("Child_pid: %d, parent_pid: %d\n", getpid(), getppid());
			printf("Response from parent pid : %s\n", msg);
		}
	} else {
        handle_error("fork()");
    }
	return 0;

}

