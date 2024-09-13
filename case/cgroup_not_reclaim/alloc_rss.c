
#define _GNU_SOURCE
#include <sched.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/mman.h>

#define CPU_NUM		64
//#define CPU_NUM		8

//#define DEBUG
#ifdef DEBUG
#define pr_deb(...)		printf( __VA_ARGS__)
#else
#define pr_deb(...)		{}
#endif

unsigned long page_size;

#define BATCH_PAGE_NUM	30
unsigned int alloc_cpu = 0;

char *g_buf;

int modify_exec_cpu(int target_cpu)
{
	cpu_set_t cpu_set;
	int new_cpu;
	
	CPU_ZERO(&cpu_set);
	CPU_SET(target_cpu, &cpu_set);
	
	pid_t pid = getpid();

	if (sched_setaffinity(pid, sizeof(cpu_set_t), &cpu_set) != 0) {
		perror("sched_setaffinity");
		return -1;
	}
	
	return 0;
}

int malloc_g_buf(size_t size)
{
	void *addr = mmap(NULL, size, PROT_READ | PROT_WRITE,
	             MAP_PRIVATE | MAP_ANONYMOUS, -1, 0);	

	if (!addr) {
		perror("mmap  error\n");
		return -1;
	}
	g_buf = addr;
	alloc_cpu = sched_getcpu();
	pr_deb("alloc cpu (%d) g_buf(%lx) g_buf_end(%lx)\n", alloc_cpu, (unsigned long)g_buf,
			(unsigned long)(g_buf + size));
	return 0;
}
void get_page_size()
{
	page_size = getpagesize();
}
void free_mem(char *addr, size_t size)
{
	int ret = madvise(addr, size, MADV_DONTNEED);
	if (ret < 0) {
		perror("madvise error");
	}
	return;
}
void let_other_cpu_free()
{
	int i;
	char *cur_ptr = g_buf;
	size_t free_size = BATCH_PAGE_NUM * page_size;

	for (i = 0; i < CPU_NUM; i++) {
		modify_exec_cpu(i);
		pr_deb("free cpu (%d) free_addr(%lx)\n", i, (unsigned long)cur_ptr);
		free_mem(cur_ptr, free_size);
		cur_ptr += free_size;
	}
	pr_deb("free end ptr(%lx)\n", (unsigned long)cur_ptr);
}
void access_page(size_t alloc_size)
{
	char *end_ptr = g_buf + alloc_size;
	char *cur_ptr = g_buf;
	while (cur_ptr < end_ptr) {
		*cur_ptr = ' ';
		cur_ptr += page_size;
	}
}
int main() 
{
	size_t alloc_size;
	int ret;

	get_page_size();
	alloc_size = CPU_NUM * BATCH_PAGE_NUM * page_size;

	pr_deb("cpu num(%d) alloc_size(%d) per_cpu_size(%d) page_size(%d)\n",
			CPU_NUM, alloc_size, BATCH_PAGE_NUM * page_size, page_size);

 	modify_exec_cpu(90);
	ret = malloc_g_buf(alloc_size);

	if (ret < 0) {
		printf("alloc g buf error\n");
		goto err;
	}

	access_page(alloc_size);
	let_other_cpu_free();

	return 0;
err:
	return -1;
}
