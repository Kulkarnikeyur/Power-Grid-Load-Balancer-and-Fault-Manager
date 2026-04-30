#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>

#include "client_utils.h"
#include "operator.h"
#include "monitor.h"
#include "signal_handler.h"
#include "admin.h"

#define SERVER_IP "127.0.0.1"
#define SERVER_PORT 8080

int main()
{
    setup_signal_handler();
    int choice;

    printf("Select role:\n");
    printf("1. Operator\n");
    printf("2. Monitor\n");
    printf("3. Admin\n");
    printf("Enter choice: ");
    scanf("%d", &choice);

    // 🔹 Operator
    if (choice == 1)
    {
        int sock = connect_to_server(SERVER_IP, SERVER_PORT);
        if (sock < 0)
        {
            printf("Failed to connect to server\n");
            return 1;
        }

        run_operator(sock);
    }

    // 🔹 Monitor
    else if (choice == 2)
    {
        run_monitor();
    }

    // 🔹 Admin
    else if (choice == 3)
    {
        char password[50];

        printf("Enter admin password: ");
        scanf("%s", password);

        if (strcmp(password, "admin123") != 0)
        {
            printf("Authentication failed!\n");
            return 0;
        }

        int sock = connect_to_server(SERVER_IP, SERVER_PORT);
        if (sock < 0)
        {
            printf("Failed to connect to server\n");
            return 1;
        }

        run_admin(sock);
    }
    else
    {
        printf("Invalid choice\n");
    }

    return 0;
}