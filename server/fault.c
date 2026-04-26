#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <signal.h>
#include <errno.h>

#include "fault.h"

#define FIFO_PATH "/tmp/grid_fault_pipe"

void init_fault_system()
{
    struct stat st;
    if (stat(FIFO_PATH, &st) == 0)
    {
        if (!S_ISFIFO(st.st_mode))
        {
            fprintf(stderr, "%s exists and is not a FIFO\n", FIFO_PATH);
            exit(EXIT_FAILURE);
        }
        return;
    }

    if (mkfifo(FIFO_PATH, 0666) == -1)
    {
        perror("mkfifo failed");
    }
    else
    {
        printf("FIFO created at %s\n", FIFO_PATH);
    }
}

void send_fault_message(const char *msg)
{
    int fd = open(FIFO_PATH, O_WRONLY | O_NONBLOCK);
    if (fd < 0)
    {
        if (errno == ENXIO)
            fprintf(stderr, "No listener for fault FIFO: %s\n", FIFO_PATH);
        else
            perror("FIFO open failed");
        return;
    }

    ssize_t written = write(fd, msg, strlen(msg));
    if (written < 0)
        perror("FIFO write failed");
    else if ((size_t)written < strlen(msg))
        fprintf(stderr, "Partial fault message write\n");

    write(fd, "\n", 1);
    close(fd);

    printf("Fault message sent: %s\n", msg);
}

void send_overload_signal(int pid)
{
    if (kill(pid, SIGUSR1) == -1)
    {
        perror("Signal send failed");
    }
    else
    {
        printf("SIGUSR1 sent to process %d\n", pid);
    }
}
