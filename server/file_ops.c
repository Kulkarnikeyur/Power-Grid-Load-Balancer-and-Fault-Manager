// server/file_ops.c

#include <stdio.h>
#include <unistd.h>
#include <fcntl.h>
#include <string.h>
#include <errno.h>
#include <time.h>
#include "file_ops.h"

#define FILE_NAME "grid_log.dat"

void log_grid_action(int client_id, const char *action, int amount, int remaining)
{
    int fd = open(FILE_NAME, O_WRONLY | O_CREAT | O_APPEND, 0644);
    if (fd < 0)
    {
        perror("File open failed");
        return;
    }

    struct flock lock = {0};
    lock.l_type = F_WRLCK;
    lock.l_whence = SEEK_SET;

    fcntl(fd, F_SETLKW, &lock);

    // Get timestamp
    time_t now = time(NULL);
    struct tm *t = localtime(&now);

    dprintf(fd, "[%02d:%02d:%02d] Client %d %s %d → Remaining: %d\n",
            t->tm_hour, t->tm_min, t->tm_sec,
            client_id, action, amount, remaining);

    lock.l_type = F_UNLCK;
    fcntl(fd, F_SETLK, &lock);

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
