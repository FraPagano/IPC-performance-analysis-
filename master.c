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
#include <time.h>
#include <math.h>

int pid_producer_named;
int pid_consumer_named;
int pid_producer_unnamed;
int pid_consumer_unnamed;
int pid_producer_sockets;
int pid_consumer_sockets;
int pid_producer_shm;
int pid_consumer_shm;
int input_size;

/* FUNCTIONS HEADERS */
int spawn(const char *program, char **arg_list);

/* FUNCTIONS */
int spawn(const char *program, char **arg_list)
{
    /* Function to generate a child process, it returns the PID of the child. */

    pid_t child_pid = fork();
    if (child_pid != 0) // Father process.
        return child_pid;

    else
    { // Child process.
        execvp(program, arg_list);
        perror("exec failed"); // If it's executed, an error occurred.
        return -4;
    }
}

void create_fifo(const char *name)
{
    /* Function to generate a named pipe. */
    mkfifo(name, 0666);
}

int interpreter()
{
    float data_size;
    int c;
    printf("\nInsert how many MB do you want to send or 'q' to quit\n");
    scanf("%f", &data_size);

    if (data_size > 100)
    {
        printf("Too many MB\n");
    }
    else
    {
        input_size = data_size * 250000;
        printf("Choose a modality: \n");
        printf(" [1] Named pipe \n [2] unnamed pipe \n [3] sockets \n [4] shared memory \n [q] for exiting\n");
        scanf("%d", &c);
        fflush(stdout);

        if (c == 1) // 1
        {
            printf("Transfering data through named pipe...\n");
            fflush(stdout);

        }
        else if (c == 2) // 2
        {
            printf("Transfer data through unnamed pipe\n");
        }
        else if (c == 3) // 3
        {
            printf("Transfer data through sockets\n");
        }
        else if (c == 4) // 4
        {
            printf("Transfer data through shared memory\n");
        }
        else
        {
            printf("Please, use the commands above\n");
        }
    }
    return c;
}

/* MAIN */
int main()
{

    char *fifo_time_start = "/tmp/fifo_time_start";
    char *fifo_time_end = "/tmp/fifo_time_end";
    create_fifo("/tmp/fifo_time_start");
    create_fifo("/tmp/fifo_time_end");
    int fd_time_start, fd_time_end;

    double start, end;
    // long int start, end;
    while (1)
    {
        int command = interpreter();
        if (command == 1) // named pipe
        {
            create_fifo("/tmp/fifo_p_to_c");
            char input_size_char[20];
            sprintf(input_size_char, "%d", input_size);
            char *arg_list_producer_named[] = {"./producer_named", input_size_char, (char *)NULL};
            pid_producer_named = spawn("./producer_named", arg_list_producer_named);
            char *arg_list_consumer_named[] = {"./consumer_named", input_size_char, (char *)NULL};
            pid_consumer_named = spawn("./consumer_named", arg_list_consumer_named);

            int prod_status, cons_status;
            fd_time_start = open(fifo_time_start, O_RDONLY);
            fd_time_end = open(fifo_time_end, O_RDONLY);
            waitpid(pid_producer_named, &prod_status, 0);
            waitpid(pid_consumer_named, &cons_status, 0);
            fflush(stdout);

            read(fd_time_start, &start, sizeof(start));
            read(fd_time_end, &end, sizeof(end));

            printf("Named pipe took: %f milliseconds\n", end - start);

            close(fd_time_start);
            close(fd_time_end);
            printf("\nProducer exited with status %d\n", prod_status);
            printf("\nConsumer exited with status %d\n", cons_status);

            unlink("/tmp/fifo_p_to_c");
        }
        if (command == 2) // unnamed pipe
        {
            int fd_unnamed[2];
            pipe(fd_unnamed);

            char input_size_char[20];
            sprintf(input_size_char, "%d", input_size);

            char input_fd_char[20];
            sprintf(input_fd_char, "%d", fd_unnamed[1]);

            char *arg_list_producer[] = {"./producer_unnamed", input_size_char,input_fd_char, (char *)NULL};
            int pid_producer = spawn("./producer_unnamed", arg_list_producer);

            sprintf(input_fd_char, "%d", fd_unnamed[0]);

            char *arg_list_consumer[] = {"./consumer_unnamed", input_size_char, input_fd_char, (char *)NULL};
            int pid_consumer = spawn("./consumer_unnamed", arg_list_consumer);

            int prod_status, cons_status;

            fd_time_start = open(fifo_time_start, O_RDONLY);
            fd_time_end = open(fifo_time_end, O_RDONLY);

            waitpid(pid_producer_named, &prod_status, 0);
            waitpid(pid_consumer_named, &cons_status, 0);

            read(fd_time_start, &start, sizeof(start));
            read(fd_time_end, &end, sizeof(end));

            printf("Unnamed pipe took: %f milliseconds\n", end - start);

            close(fd_time_start);
            close(fd_time_end);

            printf("\nProducer exited with status %d\n", prod_status);
            printf("\nConsumer exited with status %d\n", cons_status);

            unlink("/tmp/fifo_p_to_c");
        }




        // if (command == 113)
        // {
        //     break;
        // }
    }
    unlink("/tmp/fifo_time_start");
    unlink("/tmp/fifo_time_end");

    return 0;
}