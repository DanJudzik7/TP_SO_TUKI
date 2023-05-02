#ifndef HANDLER_PCB_CPU_H
#define HANDLER_PCB_CPU_H


#include <stdio.h>
#include "shared_utils.h"
#include "handler_execute_instructions.h"

void instruction_cycle(t_pcb* t_pcb);

t_list* fetch(t_pcb* t_pcb);

op_code decode(t_list* next_instruction);

void execute(op_code COD_OP,t_list* instruction,t_pcb* t_pcb);

#endif