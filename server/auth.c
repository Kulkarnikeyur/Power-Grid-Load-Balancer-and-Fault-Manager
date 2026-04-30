#include <stdio.h>
#include <string.h>
#include "auth.h"

// Convert role string → integer
int get_role_id(const char *role_str)
{
    if (strcmp(role_str, "admin") == 0)
        return ROLE_ADMIN;
    else if (strcmp(role_str, "operator") == 0)
        return ROLE_OPERATOR;
    else if (strcmp(role_str, "monitor") == 0)
        return ROLE_MONITOR;

    return -1; // invalid role
}

// Check if role can perform action
int is_authorized(int role, int action)
{

    if (role == ROLE_ADMIN)
    {
        if (action == VIEW_STATUS || action == MODIFY_CAPACITY)
            return 1;
        return 0;
    }

    if (role == ROLE_OPERATOR)
    {
        if (action == LOAD_UPDATE || action == REQUEST_CAPACITY)
            return 1;
        return 0;
    }

    if (role == ROLE_MONITOR)
    {
        if (action == VIEW_STATUS)
            return 1;
        return 0;
    }

    return 0;
}