#ifndef ACTIVE_USERS_H
#define ACTIVE_USERS_H

#include "utils.h"

typedef struct ActiveUserList{
    char* active_users[MAX_LOGGED_IN_USERS];
    int count;
} ActiveUserList;

ActiveUserList get_active_users();
void update_users(ActiveUserList* users);

#endif //ACTIVE_USERS_H
