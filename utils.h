#ifndef UTILS_H
#define UTILS_H

#define MAX_PROCESSES 65536
#define MAX_CHARS_USERNAME 512
#define MAX_LOGGED_IN_USERS 128

#include <stdio.h>
FILE* open_pipe(char* command);

#endif //UTILS_H
