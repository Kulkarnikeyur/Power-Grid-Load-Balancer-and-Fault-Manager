// server/handler.c

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <errno.h>
#include <sys/socket.h>

#include "handler.h"
#include "auth.h"
#include "grid_state.h"
#include "fault.h"
#include "../common/protocol.h"

#define BUFFER_SIZE 1024

static ssize_t recv_all(int sock, void *buffer, size_t length)
{
    size_t total = 0;
    char *ptr = buffer;

    while (total < length)
    {
        ssize_t ret = recv(sock, ptr + total, length - total, 0);
        if (ret <= 0)
            return ret;
        total += ret;
    }

    return total;
}

static ssize_t send_all(int sock, const char *buffer, size_t length)
{
    size_t total = 0;

    while (total < length)
    {
        ssize_t sent = send(sock, buffer + total, length - total, 0);
        if (sent <= 0)
            return sent;
        total += sent;
    }

    return total;
}

static void send_response(int client_socket, const char *response)
{
    size_t len = strlen(response);
    if (len == 0)
        return;

    if (send_all(client_socket, response, len) <= 0)
        perror("Failed to send response");
}

void *client_handler(void *arg)
{
    int client_socket = *(int *)arg;
    free(arg);

    Message msg;
    ssize_t bytes_read;

    printf("Handler thread started for client %d\n", client_socket);

    while (1)
    {
        bytes_read = recv_all(client_socket, &msg, sizeof(Message));
        if (bytes_read == 0)
        {
            printf("Client %d disconnected\n", client_socket);
            break;
        }
        if (bytes_read < 0)
        {
            perror("Receive failed");
            break;
        }

        printf("Received: type=%d, id=%d, role=%d, value=%d\n",
               msg.type, msg.client_id, msg.role, msg.value);

        if (!is_authorized(msg.role, msg.type))
        {
            send_response(client_socket, "ERROR: Unauthorized action\n");
            continue;
        }

        char response[BUFFER_SIZE];
        int result;

        switch (msg.type)
        {
        case REQUEST_CAPACITY:
            if (msg.value <= 0)
            {
                snprintf(response, BUFFER_SIZE,
                         "ERROR: Request amount must be positive\n");
            }
            else if (msg.value > get_total_capacity())
            {
                snprintf(response, BUFFER_SIZE,
                         "ERROR: Requested amount exceeds total capacity\n");
            }
            else
            {
                result = request_capacity(msg.value);
                if (result == 0)
                {
                    snprintf(response, BUFFER_SIZE,
                             "SUCCESS: Allocated %d units\n", msg.value);
                }
                else
                {
                    snprintf(response, BUFFER_SIZE,
                             "FAIL: Not enough capacity\n");
                    send_fault_message("ALERT: Capacity request denied due to overload");
                }
            }
            break;

        case LOAD_UPDATE:
            if (msg.value < 0)
            {
                snprintf(response, BUFFER_SIZE,
                         "ERROR: Load update value must not be negative\n");
            }
            else
            {
                snprintf(response, BUFFER_SIZE,
                         "LOAD UPDATED: %d\n", msg.value);
            }
            break;

        case VIEW_STATUS:
            snprintf(response, BUFFER_SIZE,
                     "STATUS: %d / %d used\n",
                     get_used_capacity(), get_total_capacity());
            break;

        case MODIFY_CAPACITY:
            if (msg.value <= 0)
            {
                snprintf(response, BUFFER_SIZE,
                         "ERROR: Capacity must be positive\n");
            }
            else if (msg.value < get_used_capacity())
            {
                snprintf(response, BUFFER_SIZE,
                         "FAIL: New capacity cannot be lower than used capacity\n");
            }
            else
            {
                result = modify_capacity(msg.value);
                if (result == 0)
                {
                    snprintf(response, BUFFER_SIZE,
                             "SUCCESS: Total capacity updated to %d\n", msg.value);
                }
                else
                {
                    snprintf(response, BUFFER_SIZE,
                             "FAIL: Unable to modify capacity\n");
                }
            }
            break;

        case FORCE_DISCONNECT:
            send_response(client_socket, "NOTICE: Server will close this connection\n");
            printf("Force disconnect requested by client %d\n", client_socket);
            close(client_socket);
            return NULL;

        default:
            snprintf(response, BUFFER_SIZE,
                     "ERROR: Unknown request type %d\n", msg.type);
            break;
        }

        send_response(client_socket, response);
    }

    close(client_socket);
    return NULL;
}
