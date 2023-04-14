#ifndef INSTRUCTION_HANDLER_H
#define INSTRUCTION_HANDLER_H


#include <stdio.h>
#include "serialize_utils_kernel.h"
#include "socket_utils_kernel.h"
#include "shared_utils.h"

int instruction_handler_console(int cliente_fd);
void iterator(char* value);

#endif