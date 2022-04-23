#include <stdio.h>
#include <unistd.h>
#include <sys/wait.h>
#include <stdlib.h>
#include <sys/types.h>

#include <pthread.h>
#include <string.h>

pthread_t threadID1, threadID2;

typedef struct
{
	char name[30];
	int birthyear;
	char phone[10];
	char country[10];
} human_t;

static void *thread_handle(void* arg)
{
	pthread_t thread_id;

	thread_id = pthread_self();

	human_t *human = (human_t *)arg;

	if(pthread_equal(thread_id, threadID1))
	{
		printf("This is thread 1\n");
	}else if(pthread_equal(thread_id, threadID2))
	{
		printf("This is thread 2\n");
		printf("Ho va ten: %s\n",human->name);
		printf("SDT: %s\n",human->phone);
		printf("Nam sinh: %d\n",human->birthyear);
		printf("Quoc Tich: %s\n",human->country);
	}

}

void main()
{

	int ret;
	human_t human1 = {0};

	ret = pthread_create(&threadID1, NULL, thread_handle, NULL);
	if(ret){
		printf("Error create thread1\n");
		exit(-1);
	}
	sleep(2);

	strncpy(human1.name,"Tran The Hung",sizeof(human1.name));
	strncpy(human1.phone,"012345678",sizeof(human1.phone));
	strncpy(human1.country,"Viet Nam",sizeof(human1.country));
	human1.birthyear = 1998;
	ret = pthread_create(&threadID2, NULL, thread_handle, &human1);
	if(ret)
		printf("Error create thread1");
}
