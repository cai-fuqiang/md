#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>
#include <time.h>

#define MAX_CHILDREN 10

int main() {
    printf("Parent PID: %d\n", getpid());
    srand(time(NULL) ^ getpid());

    int active_children = 0;
        int i = 0;

    while (1) {
        if (active_children < MAX_CHILDREN) {
            pid_t pid = fork();
            if (pid == 0) {
                int load = rand() % 1000000 + 500000;
                volatile int sum = 0;
                for (i = 0; i < load; ++i) {
                    sum += i;
                }
                usleep(10000);
                exit(0);
            } else if (pid > 0) {
                active_children++;
            } else {
                perror("fork");
                exit(1);
            }
        }

        int status;
        pid_t wpid;
        while ((wpid = waitpid(-1, &status, WNOHANG)) > 0) {
            active_children--;
        }

        usleep(10000);
    }
    return 0;
}

