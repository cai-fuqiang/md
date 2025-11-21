#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <math.h>
#include <sys/time.h>
#include <pthread.h>
#include <time.h>

#define THREAD_NUM 14

void busy_work(double seconds, int tid) {
    struct timeval start, now;
    gettimeofday(&start, NULL);

    double elapsed = 0;
    int i = 0;
    double x = 0;

    while (elapsed < seconds) {
        for (i = 0; i < 100000; i++) {
            x += sqrt(i * 1.0 + tid); // 避免所有线程都一样
        }
        gettimeofday(&now, NULL);
        elapsed = (now.tv_sec - start.tv_sec) + (now.tv_usec - start.tv_usec) / 1000000.0;
    }
}

void* thread_func(void* arg) {
    int tid = *(int*)arg;
    unsigned int seed = time(NULL) ^ tid;
    while (1) {
        double run_x = (rand_r(&seed) % 1000) / 10000.0; // 0.000 ~ 0.00999
        double sleep_x = (rand_r(&seed) % 1000) / 100000.0; // 0.000 ~ 0.00999
        double run_time = 0.1 + run_x;
        double sleep_time = 0.01 + sleep_x;

        printf("线程%d: CPU压力 %.5f 秒，睡眠 %.5f 秒\n", tid, run_time, sleep_time);
        busy_work(run_time, tid);

        usleep((useconds_t)(sleep_time * 1000000));
    }
    return NULL;
}

int main() {
    pthread_t threads[THREAD_NUM];
    int tids[THREAD_NUM];
    int i = 0;

    for (i = 0; i < THREAD_NUM; i++) {
        tids[i] = i;
        pthread_create(&threads[i], NULL, thread_func, &tids[i]);
    }

    for (i = 0; i < THREAD_NUM; i++) {
        pthread_join(threads[i], NULL);
    }

    return 0;
}

