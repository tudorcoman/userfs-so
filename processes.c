//
// Created by Tudor Coman on 20.12.2022.
//

#include "processes.h"
#include <stdio.h>
#include <string.h>

void get_processes(int pids[MAX_PROCESSES], char* user) {
    char command[40 + MAX_CHARS_USERNAME];
    sprintf(command, "ps -u %s | awk '{print $1}' | tail -n +2", user);

    FILE *pipe_output_comanda = open_pipe(command);

    int pid_curent;
    memset(pids, 0, sizeof pids);
    while(fscanf(pipe_output_comanda, "%d", &pids[++ pids[0]]) != EOF);

    if (pclose(pipe_output_comanda) != 0)
        fprintf(stderr," Error: Failed to close command stream \n");
}
