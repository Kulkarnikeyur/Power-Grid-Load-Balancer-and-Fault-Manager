#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <errno.h>
#include <arpa/inet.h>
#include <pthread.h>

#include "handler.h"
#include "fault.h"
#include "grid_state.h"
#include "file_ops.h"

#define PORT 8080
#define MAX_CLIENTS 10
#define DEFAULT_CAPACITY 100

int main()
{
    int server_fd, client_socket;
    struct sockaddr_in server_addr, client_addr;
    socklen_t addr_len = sizeof(client_addr);
    int reuse = 1;

    server_fd = socket(AF_INET, SOCK_STREAM, 0);
    if (server_fd < 0)
    {
        perror("Socket failed");
        exit(EXIT_FAILURE);
    }

    if (setsockopt(server_fd, SOL_SOCKET, SO_REUSEADDR, &reuse, sizeof(reuse)) < 0)
    {
        perror("setsockopt SO_REUSEADDR failed");
        close(server_fd);
        exit(EXIT_FAILURE);
    }

    printf("Socket created successfully\n");

    server_addr.sin_family = AF_INET;
    server_addr.sin_addr.s_addr = INADDR_ANY;
    server_addr.sin_port = htons(PORT);
    memset(server_addr.sin_zero, 0, sizeof(server_addr.sin_zero));

    if (bind(server_fd, (struct sockaddr *)&server_addr, sizeof(server_addr)) < 0)
    {
        perror("Bind failed");
        close(server_fd);
        exit(EXIT_FAILURE);
    }

    printf("Bind successful\n");

    if (listen(server_fd, MAX_CLIENTS) < 0)
    {
        perror("Listen failed");
        close(server_fd);
        exit(EXIT_FAILURE);
    }

    printf("Server listening on port %d...\n", PORT);

    init_fault_system();

    int loaded_used = 0, loaded_total = 0;
    if (read_grid_state(&loaded_used, &loaded_total) == 0 && loaded_total > 0)
    {
        if (loaded_used < 0)
            loaded_used = 0;
        if (loaded_used > loaded_total)
            loaded_used = loaded_total;

        init_grid(loaded_total);
        if (loaded_used > 0)
        {
            if (request_capacity(loaded_used) != 0)
            {
                fprintf(stderr, "Warning: Failed to restore used capacity state\n");
            }
        }
        printf("Loaded existing grid state: %d/%d\n", loaded_used, loaded_total);
    }
    else
    {
        init_grid(DEFAULT_CAPACITY);
        printf("Initialized grid with default capacity %d\n", DEFAULT_CAPACITY);
    }

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
               inet_ntoa(client_addr.sin_addr), ntohs(client_addr.sin_port));

        pthread_t tid;
        int *new_sock = malloc(sizeof(int));
        if (!new_sock)
        {
            perror("Memory allocation failed");
            close(client_socket);
            continue;
        }

        *new_sock = client_socket;

        if (pthread_create(&tid, NULL, client_handler, new_sock) != 0)
        {
            perror("Thread creation failed");
            close(client_socket);
            free(new_sock);
            continue;
        }

        pthread_detach(tid);
    }

    close(server_fd);
    return 0;
}