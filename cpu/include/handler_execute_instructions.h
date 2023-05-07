#ifndef HANDLER_EXECUTE_INSTRUCTIONS_H
#define HANDLER_EXECUTE_INSTRUCTIONS_H

#include <stdio.h>
#include "shared_utils.h"

void execute_set(execution_context* execution_context, t_list* instruction);
void execute_exit(execution_context* execution_context);

#endif