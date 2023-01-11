//
// Created by Tudor Coman on 20.12.2022.
//

#include "utils.h"

FILE* open_pipe(char* command) {
    return popen(command, "r");
}
