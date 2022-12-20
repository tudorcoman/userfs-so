#include "active_users.h"

#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <pthread.h>

ActiveUserList get_active_users() {
    char* command = "who | awk '{print $1}' | sort -u";
    char data[MAX_CHARS_USERNAME]; // presupunem ca un nume de user are maxim 512 caractere

    FILE *pipe_output_comanda = open_pipe(command);

    // Error handling

    ActiveUserList x;
    int i = 0;
    // Get the data from the process execution
    while(fgets(data, MAX_CHARS_USERNAME, pipe_output_comanda)) {
        // in data avem fiecare user
        x.active_users[i] = (char*) malloc(strlen(data) * sizeof(char));
        int n = strlen(data);
        data[n - 1] = '\0';
        strcpy(x.active_users[i], data);
        i ++;
    }

    if (pclose(pipe_output_comanda) != 0)
        fprintf(stderr," Error: Failed to close command stream \n");

    x.count = i;
    return x;
}

void update_users(ActiveUserList* users) {
    ActiveUserList x = get_active_users();
    users->count = x.count;
    int i;
    for (i = 0; i < x.count; i ++)
	users->active_users[i] = x.active_users[i];
    for (i = x.count + 1; i < MAX_LOGGED_IN_USERS; i ++)
	users->active_users[i] = NULL;
}


