#include <sys/un.h>
#include <sys/socket.h>
#include <stddef.h>
#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include <fcntl.h>
#include <unistd.h>

#define SOCK_PATH	"./sock_dgram"
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
	struct sockaddr_un svaddr;
    size_t send_bytes, recv_bytes;	
	char buf[BUF_SIZE]={0};
	
	
	if (signal(SIGINT, sig_handler1) == SIG_ERR) {
		printf("Cannot handle SIGINT\n");
		exit(EXIT_FAILURE);
	}

	sockfd = socket(AF_UNIX, SOCK_DGRAM, 0);

	memset(&svaddr, 0, sizeof(struct sockaddr_un)); 
	svaddr.sun_family = AF_UNIX;
	strncpy(svaddr.sun_path, SOCK_PATH, sizeof(svaddr.sun_path)-1);

	int optval;
	optval = 1;
    setsockopt(sockfd, SOL_SOCKET, SO_PASSCRED, &optval, sizeof(optval));

	send_bytes = strlen(argv[1]);

	if(sendto(sockfd, argv[1], send_bytes, 0, (struct sockaddr *) &svaddr, sizeof(struct sockaddr_un)) != send_bytes )
	{
		printf("Cannot send \n");
		return 1;
	}

	recv_bytes = recvfrom(sockfd, buf, BUF_SIZE, 0, NULL, NULL);

	if (recv_bytes == -1)
			return 1;
	else
		printf("Response : %s\n", buf);
    
    return 0;

}

