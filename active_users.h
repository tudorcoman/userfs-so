#ifndef ACTIVE_USERS_H
#define ACTIVE_USERS_H

#include "utils.h"

typedef struct ActiveUserList{
    char* active_users[MAX_LOGGED_IN_USERS]; // lista de useri activi
    int count; // numarul de useri activi
} ActiveUserList;

ActiveUserList get_active_users(); // returneaza lista de useri activi
void update_users(ActiveUserList* users); // actualizeaza lista de useri activi

#endif //ACTIVE_USERS_H
