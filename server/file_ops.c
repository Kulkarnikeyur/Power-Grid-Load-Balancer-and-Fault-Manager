// server/file_ops.c

#include <stdio.h>
#include <unistd.h>
#include <fcntl.h>
#include <string.h>
#include <errno.h>
#include "file_ops.h"

#define FILE_NAME "grid_state.dat"

// Write state to file with write lock
void write_grid_state(int used, int total)
{
    int fd = open(FILE_NAME, O_WRONLY | O_CREAT | O_TRUNC, 0644);
    if (fd < 0)
    {
        perror("File open failed");
        return;
    }

    struct flock lock;
    memset(&lock, 0, sizeof(lock));
    lock.l_type = F_WRLCK;
    lock.l_whence = SEEK_SET;

    if (fcntl(fd, F_SETLKW, &lock) < 0)
    {
        perror("fcntl lock failed");
        close(fd);
        return;
    }

    if (dprintf(fd, "USED=%d\nTOTAL=%d\n", used, total) < 0)
    {
        perror("Write failed");
    }

    lock.l_type = F_UNLCK;
    if (fcntl(fd, F_SETLK, &lock) < 0)
    {
        perror("Unlock failed");
    }

    close(fd);
}

int read_grid_state(int *used, int *total)
{
    int fd = open(FILE_NAME, O_RDONLY);
    if (fd < 0)
    {
        if (errno != ENOENT)
            perror("File open failed");
        return -1;
    }

    struct flock lock;
    memset(&lock, 0, sizeof(lock));
    lock.l_type = F_RDLCK;
    lock.l_whence = SEEK_SET;

    if (fcntl(fd, F_SETLKW, &lock) < 0)
    {
        perror("fcntl read lock failed");
        close(fd);
        return -1;
    }

    FILE *fp = fdopen(fd, "r");
    if (!fp)
    {
        perror("fdopen failed");
        close(fd);
        return -1;
    }

    if (fscanf(fp, "USED=%d\nTOTAL=%d\n", used, total) != 2)
    {
        fclose(fp);
        return -1;
    }

    lock.l_type = F_UNLCK;
    if (fcntl(fd, F_SETLK, &lock) < 0)
    {
        perror("Unlock failed");
    }

    fclose(fp);
    return 0;
}
