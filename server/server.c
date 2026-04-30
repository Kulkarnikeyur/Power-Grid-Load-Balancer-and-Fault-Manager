#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <errno.h>
#include <arpa/inet.h>
#include <pthread.h>
#include <fcntl.h>

#include "handler.h"
#include "fault.h"
#include "grid_state.h"

#define PORT 8080
#define MAX_CLIENTS 10
#define DEFAULT_CAPACITY 100

int main()
{
    int server_fd, client_socket;
    struct sockaddr_in server_addr, client_addr;
    socklen_t addr_len = sizeof(client_addr);
    int reuse = 1;

    // Create socket
    server_fd = socket(AF_INET, SOCK_STREAM, 0);
    if (server_fd < 0)
    {
        perror("Socket failed");
        exit(EXIT_FAILURE);
    }

    setsockopt(server_fd, SOL_SOCKET, SO_REUSEADDR, &reuse, sizeof(reuse));

    server_addr.sin_family = AF_INET;
    server_addr.sin_addr.s_addr = INADDR_ANY;
    server_addr.sin_port = htons(PORT);

    // Bind
    if (bind(server_fd, (struct sockaddr *)&server_addr, sizeof(server_addr)) < 0)
    {
        perror("ERROR: Server already running on this port");
        close(server_fd);
        exit(EXIT_FAILURE);
    }

    // Listen
    if (listen(server_fd, MAX_CLIENTS) < 0)
    {
        perror("Listen failed");
        close(server_fd);
        exit(EXIT_FAILURE);
    }

    int fd = open("grid_log.dat", O_WRONLY | O_CREAT | O_TRUNC, 0644);
    if (fd < 0)
    {
        perror("Failed to initialize log file");
    }
    else
    {
        close(fd);
    }
    init_fault_system();
    init_grid(DEFAULT_CAPACITY);

    printf("===== SERVER STARTED =====\n");
    printf("Listening on port %d...\n\n", PORT);

    while (1)
    {
        client_socket = accept(server_fd, (struct sockaddr *)&client_addr, &addr_len);

        if (client_socket < 0)
        {
            if (errno == EINTR)
                continue;
            perror("Accept failed");
            continue;
        }

        printf("Client %s:%d connected\n",
               inet_ntoa(client_addr.sin_addr),
               ntohs(client_addr.sin_port));

        pthread_t tid;

        client_info_t *info = malloc(sizeof(client_info_t));
        if (!info)
        {
            perror("Memory allocation failed");
            close(client_socket);
            continue;
        }

        info->socket = client_socket;
        info->addr = client_addr;

        if (pthread_create(&tid, NULL, client_handler, info) != 0)
        {
            perror("Thread creation failed");
            close(client_socket);
            free(info);
            continue;
        }

        pthread_detach(tid);
    }

    close(server_fd);
    return 0;
}