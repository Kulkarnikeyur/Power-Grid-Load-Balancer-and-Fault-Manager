#include <stdio.h>
#include <signal.h>
#include "signal_handler.h"
void handle_sigusr1(int sig)
{
    printf("\n⚠️ ALERT: Overload signal received from server!\n");
    printf("Reducing load...\n");
}

void setup_signal_handler()
{
    signal(SIGUSR1, handle_sigusr1);
}