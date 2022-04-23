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
	struct sockaddr_un svaddr, cliaddr; 
	int send_bytes,recv_bytes;
	char buf[BUF_SIZE]={0};
	socklen_t len;

	if (signal(SIGINT, sig_handler1) == SIG_ERR) {
		printf("Cannot handle SIGINT\n");
		exit(EXIT_FAILURE);
	}

	sockfd = socket(AF_UNIX, SOCK_DGRAM, 0);

	memset(&svaddr, 0, sizeof(struct sockaddr_un)); 
	svaddr.sun_family = AF_UNIX;
	strncpy(svaddr.sun_path, SOCK_PATH, sizeof(svaddr.sun_path)-1);

	if( bind(sockfd,(struct sockaddr *) &svaddr, sizeof(struct sockaddr_un) ))
	{
		printf("FAIL bind\n");
		exit(EXIT_FAILURE);
	}

	while(1)
	{
		len = sizeof(struct sockaddr_un);
		recv_bytes = recvfrom(sockfd, buf, BUF_SIZE, 0, (struct sockaddr *) &cliaddr, &len );
		if(recv_bytes == -1)
			return 1;

		printf("Server received %ld bytes from %s \n", (long) recv_bytes, cliaddr.sun_path); 
		
		for(int i=0; i < recv_bytes; i++)
		{
			buf[i] = toupper((unsigned char) buf[i]);
		}

		printf("Response: %s\n",buf);
		send_bytes = recv_bytes;
		if (sendto(sockfd, buf, send_bytes, 0, (struct sockaddr *) &cliaddr, len) != send_bytes) 
			printf("sendto error\n");
	}

}

