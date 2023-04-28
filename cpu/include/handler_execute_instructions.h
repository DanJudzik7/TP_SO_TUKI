#ifndef HANDLER_EXECUTE_INSTRUCTIONS_H
#define HANDLER_EXECUTE_INSTRUCTIONS_H


#include <stdio.h>
#include "shared_utils.h"

void set(execution_context* execution_context, t_list* instruction);

void yield(pcb* pcb);

void exit(pcb* pcb);

void dislodge(pcb* pcb);

#endif