#ifndef HANDLER_H
#define HANDLER_H
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <errno.h>
#include <arpa/inet.h>
#include <pthread.h>

typedef struct
{
    int socket;
    struct sockaddr_in addr;
} client_info_t;

void *client_handler(void *arg);

#endif