#include <sys/un.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <stddef.h>
#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include <fcntl.h> 
#include <unistd.h> 

#define BUF_SIZE	30

#define handle_error(msg) \
    do { perror(msg); exit(EXIT_FAILURE); } while (0)

int server_fd;

void sig_handler1(int num)
{
	printf("Pressed ctrl + c : %d\n",num);
	close(server_fd);
	exit(EXIT_SUCCESS);
}

void chat_func( struct sockaddr_in *svaddr)
{

	char recvbuf[BUF_SIZE]={0};
	char sendbuf[BUF_SIZE]={0};
	int recvlen, sendlen, msglen;

	while(1)
	{
		memset(recvbuf, '0', BUF_SIZE); 
		memset(sendbuf, '0', BUF_SIZE); 

		/* Nhập phản hồi từ bàn phím */
        printf("Please send the message : ");
        fgets(sendbuf, BUF_SIZE, stdin);
		msglen = strlen(sendbuf);

		sendlen = write(server_fd, sendbuf, msglen);
		if( sendlen == -1 )
			handle_error("write()");
		if( strncmp(sendbuf, "exit", 4) == 0)
		{
			printf("Client %x close connection\n",svaddr->sin_addr.s_addr);
			break;
		}
		
		recvlen = read(server_fd, recvbuf, BUF_SIZE);
		if( recvlen == -1)
			handle_error("read()");
		if( strncmp(recvbuf, "exit", 4) == 0)
		{
			printf("Server close connection\n");
			break;
		}
		printf("\nMessage from Server: %s\n",recvbuf);
	}
	close(server_fd);
}

int main(int argc, char const *argv[])
{
	int port_no;

	if (argc < 3) {
        printf("No port provided\ncommand: ./Stream_server <server address> <port number>\n");
        exit(EXIT_FAILURE);
    } else {
        port_no = atoi(argv[2]);
	}

	if (signal(SIGINT, sig_handler1) == SIG_ERR) {
		handle_error("signal()");
		exit(EXIT_FAILURE);
	}

	struct sockaddr_in svaddr; 

	server_fd = socket(AF_INET, SOCK_STREAM, 0);
	if(server_fd == -1)
	{
		handle_error("socket()");
	}

	memset(&svaddr, 0, sizeof(struct sockaddr_in)); 
	svaddr.sin_family = AF_INET;
	svaddr.sin_port = htons(port_no);
 	if (inet_pton(AF_INET, argv[1], &svaddr.sin_addr) == -1) 
        handle_error("inet_pton()");

	if(connect(server_fd, (const struct sockaddr *) &svaddr, sizeof(struct sockaddr_in)) )
		handle_error("connect()");

	chat_func(&svaddr);
	close(server_fd);

}

