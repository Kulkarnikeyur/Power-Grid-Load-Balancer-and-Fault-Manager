// client/client_utils.h

#ifndef CLIENT_UTILS_H
#define CLIENT_UTILS_H

#include "../common/protocol.h"

int connect_to_server(const char *ip, int port);

int send_message(int sock, Message *msg);

int receive_response(int sock, char *buffer, int size);

#endif