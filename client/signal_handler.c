// client/signal_handler.c

#include <stdio.h>
#include <signal.h>

// This function runs when SIGUSR1 is received
void handle_sigusr1(int sig)
{
    printf("\n⚠️ ALERT: Overload signal received from server!\n");
    printf("Reducing load...\n");

    // You can simulate load reduction here if needed
}

// Setup signal handler
void setup_signal_handler()
{
    signal(SIGUSR1, handle_sigusr1);
}