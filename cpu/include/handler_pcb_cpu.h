#ifndef HANDLER_PCB_CPU_H
#define HANDLER_PCB_CPU_H


#include <stdio.h>
#include "shared_utils.h"

void instruction_cycle(pcb* pcb);

t_list* fetch(pcb* pcb);

op_code decode(t_list* next_instruction);

void execute(op_code COD_OP,t_list* instruction,pcb* pcb);

void set(execution_context* execution_context, t_list* instruction);

#endif