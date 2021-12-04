/* LIBRARIES */
#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <errno.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <signal.h>
#include <string.h>
#include <sys/wait.h>
#include <sys/time.h>
#include <time.h>
#include <math.h>
#define MAX 2000000

int fd, fd_time_start;
char *fifo = "/tmp/fifo_p_to_c";
char *fifo_time_start = "/tmp/fifo_time_start";
struct timespec start;
int main(int argc, char *argv[])
{
    int size = atoi(argv[1]);
    int data[MAX];

    for (int i = 0; i < MAX; i++)
    {
        data[i] = rand() % 10;
    }

    fd = open(fifo, O_WRONLY);

    clock_gettime(CLOCK_REALTIME, &start);
    double time_start = start.tv_sec * 1000 + start.tv_nsec * pow(10, -6);
    printf(" producer: start time is: %f", time_start);

    int cycles = size / MAX;
    for (int i = 0; i < cycles; i++)
    {
        for (int j = 0; j < MAX; j++)
        {
            write(fd, &data[j], sizeof(int));
        }
    }

    size = size - cycles * MAX;

    for (int i = 0; i < size; i++)
    {
        write(fd, &data[i], sizeof(int));
    }

    fd_time_start = open(fifo_time_start, O_WRONLY);

    write(fd_time_start, &time_start, sizeof(time_start));

    sleep(1);

    return 0;
}