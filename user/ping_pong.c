#include "kernel/types.h"
#include "kernel/fcntl.h"
#include "kernel/stat.h"
#include "user/user.h"

#define EXCHANGE_NUMBER 10000
#define BYTES 64

int main(int argc, char *argv[]) {
    int pipe1[2];
    int pipe2[2];
    char buf[BYTES] = {};

    if (pipe(pipe1) < 0 || pipe(pipe2) < 0) {
        fprintf(2, "Couldn't create pipe\n");
        exit(1);
    }

    int pid = fork();
    if (pid < 0) {
        fprintf(2, "Couldn't fork\n");
        exit(1);
    }

    // parent process: pipe1[0] --> pipe2[1]
    // child process:  pipe2[0] --> pipe1[1]

    if (pid == 0) {
        // child process

        close(pipe1[0]);
        close(pipe2[1]);

        int start_time = uptime();

        for (int i = 0; i < EXCHANGE_NUMBER; ++i) {
            if (read(pipe2[0], buf, BYTES) != BYTES) {
                fprintf(2, "child read fail\n");
                exit(1);
            }
            if (write(pipe1[1], buf, BYTES) != BYTES) {
                fprintf(2, "child write fail\n");
                exit(1);
            }
        }

        int end_time = uptime();
        fprintf(1, "child process: %d exchanges of %d bytes in %d ticks\n",
               EXCHANGE_NUMBER, BYTES, end_time - start_time);

        close(pipe1[1]);
        close(pipe2[0]);
        exit(0);
    } else {
        // parent process

        close(pipe1[1]);
        close(pipe2[0]);

        int start_time = uptime();

        for (int i = 0; i < EXCHANGE_NUMBER; ++i) {
            if (write(pipe2[1], buf, BYTES) != BYTES) {
                fprintf(2, "parent write fail\n");
                exit(1);
            }

            if (read(pipe1[0], buf, BYTES) != BYTES) {
                fprintf(2, "parent read fail\n");
                exit(1);
            }
        }

        int end_time = uptime();

        close(pipe1[0]);
        close(pipe2[1]);
        wait(0);
        fprintf(1, "parent process: %d exchanges of %d bytes in %d ticks\n",
                EXCHANGE_NUMBER, BYTES, end_time - start_time);
    }
    exit(0);
}
