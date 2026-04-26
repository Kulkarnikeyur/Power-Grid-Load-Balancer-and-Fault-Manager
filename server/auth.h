#ifndef AUTH_H
#define AUTH_H
#include "../common/protocol.h"

int get_role_id(const char *role_str);
int is_authorized(int role, int action);

#endif