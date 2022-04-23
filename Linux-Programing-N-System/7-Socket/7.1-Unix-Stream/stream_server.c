#include <sys/un.h>
#include <sys/socket.h>
#include <stddef.h>
#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include <fcntl.h> 
#include <unistd.h>

#define SOCK_PATH	"./sock_stream"
#define BACKLOG		10
#define BUF_SIZE	30

int sockfd;
int datafd;

void sig_handler1(int num)
{
	printf("Pressed ctrl + c : %d\n",num);
	close(datafd);
	close(sockfd);
	remove(SOCK_PATH);
	exit(EXIT_SUCCESS);
}

int main(int argc, char const *argv[])
{
	struct sockaddr_un svaddr; 
	int read_num;
	char buf[BUF_SIZE]={0};
	socklen_t len;

	if (signal(SIGINT, sig_handler1) == SIG_ERR) {
		printf("Cannot handle SIGINT\n");
		exit(EXIT_FAILURE);
	}

	sockfd = socket(AF_UNIX, SOCK_STREAM, 0);

	memset(&svaddr, 0, sizeof(struct sockaddr_un)); 
	svaddr.sun_family = AF_UNIX;
	strncpy(svaddr.sun_path, SOCK_PATH, sizeof(svaddr.sun_path)-1);

	if( bind(sockfd,(struct sockaddr *) &svaddr, sizeof(struct sockaddr_un) ))
	{
		printf("FAIL bind\n");
		exit(EXIT_FAILURE);
	}
	if( listen(sockfd, BACKLOG) )
	{
		printf("FAIL listen\n");
		exit(EXIT_FAILURE);
	}


	while(1)
	{
		if( (datafd = accept(sockfd,(struct sockaddr *) &svaddr,&len) ) == -1)
		{
			printf("Fail accept\n");
			exit(EXIT_FAILURE);
		} else
		{
			printf("accept connect\n");
		}
		
		read_num = read(datafd, buf, BUF_SIZE);
		if(read_num > 0)
		{
			for(int i=0; i < read_num; i++)
			{
				buf[i] = toupper((unsigned char) buf[i]);
			}
			printf("Response: %s\n",buf);
			write(datafd,buf,BUF_SIZE);
	
		}
	}

}

