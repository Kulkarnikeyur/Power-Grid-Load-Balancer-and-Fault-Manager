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

    Message login_msg;
    login_msg.type = LOGIN;
    login_msg.client_id = getpid();
    login_msg.role = ROLE_OPERATOR; // change per file
    login_msg.value = 0;

    send_message(sock, &login_msg);

    while (1)
    {
        printf("\n--- Operator Menu ---\n");
        printf("1. Send Load Update\n");
        printf("2. Request Capacity\n");
        printf("3. Exit\n");
        printf("Enter choice: ");
        scanf("%d", &choice);

        switch (choice)
        {

        case 1:
            msg.type = LOAD_UPDATE;
            printf("Enter current load: ");
            scanf("%d", &msg.value);

            send_message(sock, &msg);
            receive_response(sock, response, sizeof(response));
            printf("Server: %s\n", response);
            break;

        case 2:
            msg.type = REQUEST_CAPACITY;
            printf("Enter required capacity: ");
            scanf("%d", &msg.value);

            send_message(sock, &msg);
            receive_response(sock, response, sizeof(response));
            printf("Server: %s\n", response);
            break;

        case 3:
            printf("Exiting operator...\n");
            close(sock);
            return;

        default:
            printf("Invalid choice\n");
        }
    }
}