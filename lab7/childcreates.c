#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <sys/wait.h>

int main(int argc, char **argv) {
    int i;
    int n;
    int iterations;
    // int count = 0;

    if (argc != 2) {
        fprintf(stderr, "Usage: forkloop <iterations>\n");
        exit(1);
    }
    iterations = strtol(argv[1], NULL, 10);
    for (i = 0; i < iterations; i++) {
        if (i == 0){
            n = fork();
        }
        if (n < 0) {
            perror("fork");
            exit(1);
        }
        if (n == 0) {
            printf("ppid = %d, pid = %d, i = %d\n", getppid(), getpid(), i);
            n = fork();
        }
    }
    int status;
    wait(&status);
    // exit(1);

    return 0;
}
