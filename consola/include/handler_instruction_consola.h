#ifndef HANDLER_INSTRUCTION_CONSOLA_H
#define HANDLER_INSTRUCTION_CONSOLA_H

#include <stdio.h>
#include <string.h>
#include "shared_utils.h"

// Lee un archivo y manda sus instrucciones al kernel
t_package* process_instructions(int socket_kernel);

t_package* serialize_instruction(char* line);

#endif