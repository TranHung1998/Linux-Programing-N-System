#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <string.h>

#define DEVICE_NODE "/dev/iomodule"
#define DEVICE_NODE1 "/dev/iomodule1"
#define DEVICE_NODE2 "/dev/iomodule2"

#define BUFF_SIZE 30
/* ham kiem tra entry point open cua vchar driver */
int open_chardev() {
    int fd = open(DEVICE_NODE, O_RDWR);
    if(fd < 0) {
        printf("Cannot open the device file\n");
        exit(1);
    }
    return fd;
}

/* ham kiem tra entry point release cua vchar driver */
void close_chardev(int fd) {
    close(fd);
}

void read_data_chardev(){
    int ret = 0;
    char user_buff[BUFF_SIZE];
    memset(user_buff, 0, BUFF_SIZE);
    int fd = open_chardev();
    ret = read(fd, user_buff, BUFF_SIZE);
    if (ret < 0)
    {
        printf("Could not read Memory register \n");
    }
    if(!ret)
    {
        printf("End of file\n");
    }
    if(ret <= BUFF_SIZE)
    {  
        printf("Successful recieve data from Hardware. Register value: %s \n",user_buff);
    }
    close(fd);
}

void write_data_chardev(){
    int ret = 0;
    char user_buff[BUFF_SIZE];
    int fd = open_chardev();
    memset(user_buff, 0, BUFF_SIZE);
    printf("Enter the Register Address: ");
    scanf(" %[^\n]s", user_buff);

    ret = write(fd, user_buff, strlen(user_buff) + 1);
    close_chardev(fd);
    if (ret < 0)
    {
        printf("Could not read Memory register \n");
    }
    if(!ret)
    {
        printf("End of file\n");
    }
    if(ret <= BUFF_SIZE)
    {
        printf("Successful send data to Hardware. Register address: %s\n",user_buff);
    }
}

int main() {
    int ret = 0;
    char option = 'q';
    int fd = -1;

    printf("Select below options:\n");
    printf("\to (to open a device node)\n");
    printf("\tc (to close the device node)\n");
    printf("\tr (to read the application)\n");
    printf("\tw (to write the application)\n");
    printf("\tq (to quit the application)\n");
    while (1) {
        printf("Enter your option: ");
        scanf(" %c", &option);

        switch (option) {
            case 'o':
                if (fd < 0)
                    fd = open_chardev();
                else
                    printf("%s has already opened\n", DEVICE_NODE);
                break;
            case 'c':
                if (fd > -1)
                    close_chardev(fd);
                else
                    printf("%s has not opened yet! Cannot close\n", DEVICE_NODE);
                fd = -1;
                break;
            case 'r':
                read_data_chardev();
                break;    
            case 'w':
                write_data_chardev();
                break;    
            case 'q':
                if (fd > -1)
                    close_chardev(fd);
                printf("Quit the application. Good bye!\n");
                return 0;
            default:
                printf("invalid option %c\n", option);
                break;
        }
    };
}