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
    login_msg.role = ROLE_OPERATOR;
    login_msg.value = 0;

    send_message(sock, &login_msg);
    receive_response(sock, response, sizeof(response));
    printf("\n[SERVER] %s\n", response);
    fflush(stdout);

    while (1)
    {
        printf("\n===== OPERATOR MENU =====\n");
        printf("1. Update Load\n");
        printf("2. Request Capacity\n");
        printf("3. Exit\n");
        printf("Enter choice: ");
        fflush(stdout);
        scanf("%d", &choice);

        switch (choice)
        {

        case 1:
            msg.role = ROLE_OPERATOR;
            msg.client_id = client_id;
            msg.type = LOAD_UPDATE;
            printf("Enter current load: ");
            fflush(stdout);
            scanf("%d", &msg.value);

            send_message(sock, &msg);
            receive_response(sock, response, sizeof(response));
            printf("\n[SERVER] %s\n", response);
            break;

        case 2:
            msg.role = ROLE_OPERATOR;
            msg.client_id = client_id;
            msg.type = REQUEST_CAPACITY;
            printf("Enter required capacity: ");
            fflush(stdout);
            scanf("%d", &msg.value);

            send_message(sock, &msg);
            receive_response(sock, response, sizeof(response));
            printf("\n[SERVER] %s\n", response);
            break;

        case 3:
            printf("Exiting operator...\n");
            close(sock);
            return;

        default:
            printf("Invalid choice");
        }
    }
}