#include <stdio.h>
#include <unistd.h>
#include <sys/wait.h>
#include <stdlib.h>
#include <sys/types.h>

#include <fcntl.h>
#include <pthread.h>
#include <string.h>

pthread_t threadID1, threadID2, threadID3;
#define MAX_BUF		30

typedef struct
{
	char name[MAX_BUF+1];
	char birthyear[MAX_BUF+1];
	char country[MAX_BUF+1];
} human_t;

human_t human = {0};

pthread_mutex_t mtx = PTHREAD_MUTEX_INITIALIZER;
pthread_mutex_t mtx2 = PTHREAD_MUTEX_INITIALIZER;

int running_thread = 0;
#define THREAD1_STATE	1
#define THREAD2_STATE	2
#define THREAD3_STATE	3

static void *thread_handle(void* arg)
{
	int fd;
	pthread_t thread_id;

	thread_id = pthread_self();
	
	char buf[MAX_BUF+1] = "";
	int numRead;
	while(1){
		if(pthread_equal(thread_id, threadID1))
		{
			pthread_mutex_lock(&mtx);
			printf( "Nhap Ho ten: ");
			scanf("%s", human.name);
			printf( "Nhap Nam sinh: ");
			scanf("%s", human.birthyear);
			printf( "Nhap Que Quan: ");
			scanf("%s", human.country);
			pthread_mutex_unlock(&mtx);
			running_thread = THREAD2_STATE;
			while(running_thread != THREAD1_STATE);
		}else if(pthread_equal(thread_id, threadID2))
		{
			pthread_mutex_lock(&mtx);
			fd = open("./thongtinsinhvien.txt", O_WRONLY);

			lseek(fd, 0, SEEK_SET);
			sprintf(buf,"%s\n",human.name);
			write(fd, buf , MAX_BUF);
			
			lseek(fd, 0, SEEK_CUR);
			sprintf(buf,"%s\n",human.birthyear);
			write(fd, buf, MAX_BUF);
			
			lseek(fd, 0, SEEK_CUR);
			sprintf(buf,"%s\n",human.country);
			write(fd, buf, MAX_BUF);
	
			close(fd);

			pthread_mutex_unlock(&mtx);

			running_thread = THREAD3_STATE;
			while(running_thread != THREAD2_STATE);

		}else if(pthread_equal(thread_id, threadID3))
		{
			pthread_mutex_lock(&mtx);
			fd = open("./thongtinsinhvien.txt", O_RDONLY);
			
			lseek(fd, 0, SEEK_SET);
			numRead = read(fd, human.name, MAX_BUF);
			human.name[numRead] = '\0';
			printf( "Ho Ten: %s",human.name);
			
			lseek(fd, 0, SEEK_CUR);
			numRead = read(fd, human.birthyear, MAX_BUF);
			human.birthyear[numRead] = '\0';
			printf( "Nam sinh: %s",human.birthyear);
			
			lseek(fd, 0, SEEK_CUR);
			numRead = read(fd, human.country, MAX_BUF);
			human.country[numRead] = '\0';
			printf( "Que Quan: %s",human.country);
			
			close(fd);
			pthread_mutex_unlock(&mtx);
			running_thread = THREAD1_STATE;
			while(running_thread != THREAD3_STATE);
		}
	}
}

void main()
{

	int ret;
	int fd;
	fd = open("thongtinsinhvien.txt" ,O_RDWR | O_CREAT, 0667);
	close(fd);
	ret = pthread_create(&threadID1, NULL, thread_handle, NULL);
	if(ret){
		printf("Error create thread1\n");
		exit(-1);
	}


	ret = pthread_create(&threadID2, NULL, thread_handle, NULL);
	if(ret){
		printf("Error create thread1");
		exit(-1);
	}
	
	ret = pthread_create(&threadID3, NULL, thread_handle, NULL);
	if(ret){
		printf("Error create thread1");
		exit(-1);
	}
	
	pthread_detach(threadID1);
	pthread_detach(threadID2);
	pthread_detach(threadID3);
	while(1);

}

