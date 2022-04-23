#include <sys/un.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <stddef.h>
#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include <fcntl.h> 
#include <unistd.h>

#define BACKLOG		10
#define BUF_SIZE	30

#define handle_error(msg) \
    do { perror(msg); exit(EXIT_FAILURE); } while (0)

int server_fd;
int client_fd;

void sig_handler1(int num)
{
	printf("Pressed ctrl + c : %d\n",num);
	close(server_fd);
	exit(EXIT_SUCCESS);
}

void chat_func( struct sockaddr_in *cliaddr)
{
	char recvbuf[BUF_SIZE]={0};
	char sendbuf[BUF_SIZE]={0};
	int recvlen, sendlen, msglen;

	while(1)
	{
		memset(recvbuf, '0', BUF_SIZE); 
		memset(sendbuf, '0', BUF_SIZE); 
		
		recvlen = read(client_fd, recvbuf, BUF_SIZE);
		
		if( recvlen == -1)
			handle_error("read()");
		if( strncmp(recvbuf, "exit", 4) == 0)
		{
			printf("Client %x close connection\n",cliaddr->sin_addr.s_addr);
			break;
		}
		printf("\nMessage from Server: %s\n",recvbuf);

		/* Nhập phản hồi từ bàn phím */
        printf("Please respond the message : ");
        fgets(sendbuf, BUF_SIZE, stdin);
		msglen = strlen(sendbuf);

		sendlen = write(client_fd, sendbuf, msglen);
		if( sendlen == -1 )
			handle_error("write()");
		if( strncmp(sendbuf, "exit", 4) == 0)
		{
			printf("Server close connection\n");
			break;
		}

		sleep(1);
	}

}


int main(int argc, char const *argv[])
{

	int port_no, opt;

	if (signal(SIGINT, sig_handler1) == SIG_ERR) {
		handle_error("signal()");
		exit(EXIT_FAILURE);
	}

	if (argc < 2) {
        printf("No port provided\ncommand: ./Stream_server <port number>\n");
        exit(EXIT_FAILURE);
    } else {
        port_no = atoi(argv[1]);
	}

	struct sockaddr_in svaddr, cliaddr; 
	socklen_t len;
	
	server_fd = socket(AF_INET, SOCK_STREAM, 0);

	if(server_fd == -1)
	{
		handle_error("socket()");
	}

	memset(&svaddr, 0, sizeof(struct sockaddr_in));
	memset(&cliaddr, 0, sizeof(struct sockaddr_in)); 

	svaddr.sin_family = AF_INET;
	svaddr.sin_port = htons(port_no);
	svaddr.sin_addr.s_addr = INADDR_ANY;

	/* Ngăn lỗi : “address already in use” */
	if (setsockopt(server_fd, SOL_SOCKET, SO_REUSEADDR | SO_REUSEPORT, &opt, sizeof(opt)))
        handle_error("setsockopt()");  

	if( bind(server_fd,(struct sockaddr *) &svaddr, sizeof(struct sockaddr_in)) == -1 )
	{
		handle_error("bind()");
	}
	if( listen(server_fd, BACKLOG) == -1 )
	{
		handle_error("listen()");
	}
	
	while(1)
	{
		printf("Server is listening at port : %d \n....\n",port_no);
		client_fd = accept(server_fd, (struct sockaddr *) &cliaddr, &len);
		if(client_fd == -1)
		{
			handle_error("accept()");
		} else
		{
			printf("accept connect\n");
		}
		printf("Server : got connection \n");
		chat_func(&cliaddr);
		sleep(1);
	}
}

