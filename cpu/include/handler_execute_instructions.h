#ifndef HANDLER_EXECUTE_INSTRUCTIONS_H
#define HANDLER_EXECUTE_INSTRUCTIONS_H

#include "shared_package.h"

void execute_set(execution_context* execution_context, t_instruction* instruction);

void execute_exit(execution_context* execution_context);

#endif