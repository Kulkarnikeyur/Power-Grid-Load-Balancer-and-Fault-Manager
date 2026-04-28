// client/client_utils.h

#ifndef CLIENT_UTILS_H
#define CLIENT_UTILS_H

#include "../common/protocol.h"

// Connect to server
int connect_to_server(const char *ip, int port);

// Send message
int send_message(int sock, Message *msg);

// Receive response
int receive_response(int sock, char *buffer, int size);

#endif