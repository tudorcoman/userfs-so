//
// Created by Tudor Coman on 20.12.2022.
//

#include "processes.h"
#include <stdio.h>
#include <string.h>

void get_processes(int pids[MAX_PROCESSES], char* user) {
    
    // intoarce pid-urile proceselor unui user
    char command[40 + MAX_CHARS_USERNAME]; //comanda are lungimea de 40 de caractere + lungimea numelui de user
    sprintf(command, "ps -u %s | awk '{print $1}' | tail -n +2", user); //comanda returnaza pid-urile proceselor unui user

    FILE *pipe_output_comanda = open_pipe(command); //deschidem pipe-ul de output al comenzii

    //initializam vectorul de pid-uri cu 0 si citim pid-urile proceselor

    int pid_curent;
    memset(pids, 0, sizeof pids);
    while(fscanf(pipe_output_comanda, "%d", &pids[++ pids[0]]) != EOF); 

    if (pclose(pipe_output_comanda) != 0)
        fprintf(stderr," Error: Failed to close command stream \n");

}