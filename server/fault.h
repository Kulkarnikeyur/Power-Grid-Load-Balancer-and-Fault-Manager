#ifndef FAULT_H
#define FAULT_H

void init_fault_system();
void send_fault_message(const char *msg);
void send_overload_signal(int pid);

#endif