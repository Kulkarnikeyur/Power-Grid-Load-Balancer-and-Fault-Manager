#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>

#include "admin.h"
#include "client_utils.h"
#include "../common/protocol.h"

void run_admin(int sock)
{
    char response[1024];
    Message msg;
    int choice;

    Message login_msg;
    login_msg.type = LOGIN;
    login_msg.client_id = getpid();
    login_msg.role = ROLE_ADMIN;
    login_msg.value = 0;

    send_message(sock, &login_msg);
    receive_response(sock, response, sizeof(response));
    printf("\n[SERVER] %s\n", response);

    while (1)
    {
        printf("\n===== ADMIN MENU =====\n");
        printf("1. View Grid Status\n");
        printf("2. Modify Capacity\n");
        printf("3. Exit\n");
        printf("Enter choice: ");
        scanf("%d", &choice);

        switch (choice)
        {

        case 1:
            msg.role = ROLE_ADMIN;
            msg.client_id = getpid();
            msg.type = VIEW_STATUS;
            msg.value = 0;

            send_message(sock, &msg);
            receive_response(sock, response, sizeof(response));
            printf("\n[SERVER] %s\n", response);
            break;

        case 2:
            msg.role = ROLE_ADMIN;
            msg.client_id = getpid();
            msg.type = MODIFY_CAPACITY;
            printf("Enter new capacity: ");
            scanf("%d", &msg.value);

            send_message(sock, &msg);
            receive_response(sock, response, sizeof(response));
            printf("\n[SERVER] %s\n", response);
            break;

        case 3:
            printf("Exiting admin...\n");
            close(sock);
            return;

        default:
            printf("Invalid choice\n");
        }
    }
}