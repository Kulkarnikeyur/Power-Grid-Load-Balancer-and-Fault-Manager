// client/operator.c

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include "operator.h"
#include "client_utils.h"
#include "../common/protocol.h"

void run_operator(int sock)
{
    int choice;
    Message msg;
    char response[1024];
    int client_id = getpid();

    Message login_msg;
    login_msg.type = LOGIN;
    login_msg.client_id = client_id;
    login_msg.role = ROLE_OPERATOR; // change per file
    login_msg.value = 0;

    send_message(sock, &login_msg);
    receive_response(sock, response, sizeof(response));
    printf("Server: %s", response);
    fflush(stdout);

    while (1)
    {
        printf("\n--- Operator Menu ---");
        printf("\n1. Update Load");
        fflush(stdout);
        printf("\n2. Request Capacity");
        fflush(stdout);
        printf("\n3. Exit");
        fflush(stdout);
        printf("\nEnter choice: ");
        fflush(stdout);
        scanf("%d", &choice);

        switch (choice)
        {

        case 1:
            msg.role = ROLE_OPERATOR;
            msg.client_id = client_id;
            msg.type = LOAD_UPDATE;
            printf("\nEnter current load: ");
            fflush(stdout);
            scanf("%d", &msg.value);

            send_message(sock, &msg);
            receive_response(sock, response, sizeof(response));
            printf("\nServer: %s", response);
            fflush(stdout);
            break;

        case 2:
            msg.role = ROLE_OPERATOR;
            msg.client_id = client_id;
            msg.type = REQUEST_CAPACITY;
            printf("\nEnter required capacity: ");
            fflush(stdout);
            scanf("%d", &msg.value);

            send_message(sock, &msg);
            receive_response(sock, response, sizeof(response));
            printf("\nServer: %s", response);
            fflush(stdout);
            break;

        case 3:
            printf("\nExiting operator...\n");
            fflush(stdout);
            close(sock);
            return;

        default:
            printf("\nInvalid choice");
            fflush(stdout);
        }
    }
}