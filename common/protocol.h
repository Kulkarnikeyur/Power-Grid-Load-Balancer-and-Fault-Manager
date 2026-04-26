#ifndef PROTOCOL_H
#define PROTOCOL_H

// Roles
#define ROLE_ADMIN 1
#define ROLE_OPERATOR 2
#define ROLE_MONITOR 3

// Message Types
#define LOAD_UPDATE 1
#define REQUEST_CAPACITY 2
#define VIEW_STATUS 3
#define MODIFY_CAPACITY 4
#define FORCE_DISCONNECT 5

// Message structure
typedef struct
{
    int type;
    int client_id;
    int role;
    int value;
} Message;

#endif