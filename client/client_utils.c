#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>

#include "client_utils.h"

int connect_to_server(const char *ip, int port)
{
    int sock;
    struct sockaddr_in server_addr;

    sock = socket(AF_INET, SOCK_STREAM, 0);
    if (sock < 0)
    {
        perror("Socket creation failed");
        return -1;
    }

    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(port);

    if (inet_pton(AF_INET, ip, &server_addr.sin_addr) <= 0)
    {
        perror("Invalid address");
        close(sock);
        return -1;
    }

    if (connect(sock, (struct sockaddr *)&server_addr, sizeof(server_addr)) < 0)
    {
        perror("Connection failed");
        close(sock);
        return -1;
    }

    printf("Connected to server %s:%d\n", ip, port);

    return sock;
}

int send_message(int sock, Message *msg)
{
    int bytes = send(sock, msg, sizeof(Message), 0);
    if (bytes < 0)
    {
        perror("Send failed");
        return -1;
    }
    return 0;
}

int receive_response(int sock, char *buffer, int size)
{
    int bytes = recv(sock, buffer, size - 1, 0);
    if (bytes < 0)
    {
        perror("Receive failed");
        return -1;
    }

    buffer[bytes] = '\0';
    return bytes;
}