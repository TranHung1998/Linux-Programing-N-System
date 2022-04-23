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

void sig_handler1(int num)
{
	printf("Pressed ctrl + c : %d\n",num);
	close(sockfd);
	exit(EXIT_SUCCESS);
}

int main(int argc, char const *argv[])
{
	struct sockaddr_un cliaddr; 
	char buf[BUF_SIZE]={0};

	if (signal(SIGINT, sig_handler1) == SIG_ERR) {
		printf("Cannot handle SIGINT\n");
		exit(EXIT_FAILURE);
	}

	sockfd = socket(AF_UNIX, SOCK_STREAM, 0);

	memset(&cliaddr, 0, sizeof(struct sockaddr_un)); 
	cliaddr.sun_family = AF_UNIX;
	strncpy(cliaddr.sun_path, SOCK_PATH, strlen(SOCK_PATH));

	if(connect(sockfd, (const struct sockaddr *) &cliaddr, sizeof(struct sockaddr_un)) )
		printf("Error connect\n");


	write(sockfd, argv[1], BUF_SIZE);
	while(1)
	{      
	        if(read(sockfd, buf, sizeof(buf)) > 0){
				printf("From Server : %s\n", buf); 
				exit(EXIT_SUCCESS);
			}
	}


}

