#include <stdio.h>
#include <string.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <sys/types.h> 
#include <unistd.h>

#define BUF_SIZE	30
#define FIFO_PATH	"./fifo_file"

int main(int argc, char const *argv[])
{   
    char buf[BUF_SIZE];
    int fd;

    mkfifo(FIFO_PATH, 0666);

    while (1) {
        // Read first
        fd = open(FIFO_PATH, O_RDONLY);
        read(fd, buf, BUF_SIZE);

        printf("producer message: %s", buf);
        close(fd);
    }

    return 0;
}
