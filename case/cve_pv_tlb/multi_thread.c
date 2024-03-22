#include <stdio.h>
#include <pthread.h>
#include <unistd.h>
#include <assert.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <sys/stat.h>

#include "list.h"

#define NUM_THREADS 3

pthread_mutex_t mtx;
LIST_HEAD(dataQueue);

#define PKGSIZE  65536
#define TEST_NUM 5

#define DATA_SIZE (PKGSIZE - sizeof(struct test))
struct test {
	struct list_head node;
	char data[];
};

struct data_bak {
	char buffer[PKGSIZE];
	char *ptr;
};

struct data_bak_st {
	struct data_bak data_bak[TEST_NUM];
};

struct data_bak_st data_bak_st;

void copy_to_data_bak_st(int i, struct test *test)
{
	memcpy(data_bak_st.data_bak[i].buffer, test, PKGSIZE);
	data_bak_st.data_bak[i].ptr = (char *)test;
}
void *producer(void *arg) {
	printf("producer\n");
	char tmpbuff[PKGSIZE];
	for(int i = 0; i < PKGSIZE; ++i)
	{
		tmpbuff[i] = 'a' + i%10;
	} 
	for (int i = 0; i < 20; i++) {
		struct test *data = (struct test *)malloc(sizeof(struct test) + sizeof(char) * (PKGSIZE + 1) );
		memcpy(data->data, tmpbuff, PKGSIZE);
		pthread_mutex_lock(&mtx);
		list_add(&data->node, &dataQueue);
		pthread_mutex_unlock(&mtx);
	}

	while (true) {
		pthread_mutex_lock(&mtx);
		if (list_empty(&dataQueue)) {
			int insernum = rand() % TEST_NUM + 1;
			for (size_t i = 0; i < insernum; i++) {
				struct test *data = (struct test *)malloc(PKGSIZE);
				memcpy(data->data, tmpbuff, DATA_SIZE);
				list_add(&data->node, &dataQueue);
				copy_to_data_bak_st(i, data);
			}
		}
		pthread_mutex_unlock(&mtx);
	}
	return NULL;
}

void init_data_bak_st()
{
	int i = 0;
	for (i = 0; i < TEST_NUM; i++) {
		memset(data_bak_st.data_bak[i].buffer, 0, PKGSIZE);
		data_bak_st.data_bak[i].ptr = NULL;
	}
}

void *consumer(void *arg) {
	printf("consumer\n");
	int conNum = 0;
	int size;

	while (true) {
		struct test *this, *tmp;
		pthread_mutex_lock(&mtx);
		list_for_each_entry_safe(this, tmp, &dataQueue, node) {
			list_del(&this->node);
			free(this);
			break;
		}
		init_data_bak_st();
		pthread_mutex_unlock(&mtx);
	}
	return NULL;
}

void create_pid_file()
{
	pid_t pid;
	pid = getpid();

	FILE *fp;

	fp = fopen("/var/run/cve_test.pid", "w");

	if (!fp) {
		printf("fopen error\n");
		exit(1);
	}
	fprintf(fp, "%d\n", pid);

	fclose(fp);
}

void daemon_my()
{
	pid_t pid;
	int i;
 
    	pid = fork();
    	if(pid < 0)
    	{
    	    printf("Error Fork\n");
    	    exit(1);
    	}
    	else if(pid > 0)
    	{
    	    exit(0);
    	}
 
    	setsid();
    	chdir("/tmp");
    	umask(0);
 
    	for(i = 0; i< getdtablesize(); i++)
    	{
    	    close(i);
    	}
	return ;
}
int main()
{
	daemon_my();
	printf("start test\n");
	create_pid_file();
	pthread_mutexattr_t attr;
#if 0
	int err = pthread_mutex_init(&mtx, &attr);
	//assert(err == 0);
	if (err != 0) {
		printf("mutex init error\n");
		exit(1);
	}
	pthread_mutexattr_destroy(&attr);
#else
	int err = pthread_mutex_init(&mtx, NULL);
	//assert(err == 0);
	if (err != 0) {
		printf("mutex init error\n");
		exit(1);
	}
#endif

	pthread_attr_t attr1;
	pthread_t thread1;
	pthread_attr_init(&attr1);
	pthread_create(&thread1, &attr1, producer, NULL);

	pthread_attr_t attr2;
	pthread_t thread2;
	pthread_attr_init(&attr2);
	pthread_create(&thread2, &attr2, consumer, NULL);


	pthread_attr_t attr3;
	pthread_t thread3;
	pthread_attr_init(&attr3);
	pthread_create(&thread3, &attr3, consumer, NULL);


	pthread_attr_t attr4;
	pthread_t thread4;
	pthread_attr_init(&attr4);
	pthread_create(&thread4, &attr4, consumer, NULL);

	void *ret;
	pthread_join(thread1, &ret);
	pthread_join(thread2, &ret);
	pthread_join(thread3, &ret);
	pthread_join(thread4, &ret);

	return 0;
}
