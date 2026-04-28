// client/monitor.c

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>

#include "monitor.h"

#define FIFO_PATH "/tmp/grid_fault_pipe"
#define BUFFER_SIZE 1024

void run_monitor()
{
    int fd;
    char buffer[BUFFER_SIZE];

    printf("Starting monitor...\n");

    // Open FIFO for reading
    fd = open(FIFO_PATH, O_RDONLY);
    if (fd < 0)
    {
        perror("FIFO open failed");
        return;
    }

    printf("Listening for fault messages...\n");

    while (1)
    {
        int bytes = read(fd, buffer, BUFFER_SIZE - 1);

        if (bytes > 0)
        {
            buffer[bytes] = '\0';
            printf("FAULT ALERT: %s", buffer);
        }
    }

    close(fd);
}