// server/handler.c

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/socket.h>

#include "handler.h"
#include "auth.h"
#include "grid_state.h"
#include "fault.h"
#include "../common/protocol.h"

#define BUFFER_SIZE 1024

// Safe receive
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

// Send response
static void send_response(int client_socket, const char *response)
{
    send(client_socket, response, strlen(response), 0);
}

void *client_handler(void *arg)
{
    client_info_t *info = (client_info_t *)arg;
    int client_socket = info->socket;
    free(info);

    int client_role = -1;
    int client_id = -1;

    Message msg;

    printf("Handler thread started for socket %d\n", client_socket);

    while (1)
    {
        ssize_t bytes_read = recv_all(client_socket, &msg, sizeof(Message));

        if (bytes_read <= 0)
        {
            printf("Client socket %d disconnected\n", client_socket);
            break;
        }

        char response[BUFFER_SIZE];
        int result;

        // 🔥 LOGIN handled FIRST (no auth check)
        if (msg.type == LOGIN)
        {
            client_role = msg.role;
            client_id = msg.client_id;

            printf("Client %d logged in as ", client_id);

            if (client_role == ROLE_ADMIN)
                printf("ADMIN\n");
            else if (client_role == ROLE_OPERATOR)
                printf("OPERATOR\n");
            else if (client_role == ROLE_MONITOR)
                printf("MONITOR\n");

            send_response(client_socket, "LOGIN OK\n");
            continue;
        }

        // 🔒 Authorization AFTER login
        if (!is_authorized(msg.role, msg.type))
        {
            send_response(client_socket, "ERROR: Unauthorized action\n");
            continue;
        }

        switch (msg.type)
        {
        case REQUEST_CAPACITY:
            if (msg.value <= 0)
            {
                snprintf(response, BUFFER_SIZE,
                         "ERROR: Request must be positive\n");
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

                    send_fault_message("ALERT: Capacity request denied");
                }
            }
            break;

        case LOAD_UPDATE:
            snprintf(response, BUFFER_SIZE,
                     "LOAD UPDATED: %d\n", msg.value);
            break;

        case VIEW_STATUS:
            snprintf(response, BUFFER_SIZE,
                     "STATUS: %d / %d used\n",
                     get_used_capacity(), get_total_capacity());
            break;

        case MODIFY_CAPACITY:
            result = modify_capacity(msg.value);

            if (result == 0)
            {
                snprintf(response, BUFFER_SIZE,
                         "SUCCESS: Capacity updated to %d\n", msg.value);
            }
            else
            {
                snprintf(response, BUFFER_SIZE,
                         "FAIL: Invalid capacity update\n");
            }
            break;

        case FORCE_DISCONNECT:
            send_response(client_socket, "Disconnected by server\n");
            close(client_socket);
            return NULL;

        default:
            snprintf(response, BUFFER_SIZE,
                     "ERROR: Unknown request\n");
        }

        send_response(client_socket, response);
    }

    close(client_socket);
    return NULL;
}