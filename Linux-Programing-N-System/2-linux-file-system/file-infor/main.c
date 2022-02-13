#include <stdio.h>
#include <stdint.h>
#include <sys/stat.h>
#include <stdlib.h>
#include <time.h>

int main(int argc, char const *argv[])
{
	struct stat sb;
	
	if( 2 != argc )
	{
		printf("Usage: ./file-infor /path/to/file\n");
		exit(EXIT_FAILURE);
	}
	
	if( -1 == lstat(argv[1], &sb) )
	{
		printf("Enter wrong path\n"); 
		exit(EXIT_FAILURE);
	} 	
	printf("File %s is a ", argv[1]);
	if (S_ISDIR(sb.st_mode) )
	{
		printf("directory\n");
	} else if ( S_ISCHR(sb.st_mode) )
	{
		printf("character\n");
	} else if ( S_ISBLK(sb.st_mode) )
	{
		printf("block\n");
	} else if ( S_ISREG(sb.st_mode) )
        {
                printf("regular\n");
        }  else if ( S_ISFIFO(sb.st_mode) )
        {
                printf("FIFO\n");
        } else if ( S_ISLNK(sb.st_mode) )
        {
                printf("LINK\n");
        } else if ( S_ISSOCK(sb.st_mode) )
        {
                printf("Socket\n");
        } else 
	{
		printf("unknow\n");
	}

	printf("\n");
	printf("Last status change:     %s\n", ctime(&sb.st_ctim.tv_sec));
	printf("Last file access:       %s\n", ctime(&sb.st_atim.tv_sec));
	printf("Last file modification: %s\n", ctime(&sb.st_mtim.tv_sec));
	
	printf("\n");
	printf("File size is:	%ld bytes\n", sb.st_size);

	return 0;
}


