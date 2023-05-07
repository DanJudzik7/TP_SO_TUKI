#ifndef HANDLER_PCB_CPU_H
#define HANDLER_PCB_CPU_H


#include "cpu.h"
#include <stdio.h>
#include "shared_utils.h"
#include "handler_execute_instructions.h"

extern configuration_cpu config_cpu;

t_pcb* fetch(t_pcb* t_pcb);
t_pcb* decode(t_pcb* t_pcb, t_list* instruction);
t_list* get_instrucction(execution_context* execution_context);
void dislodge();

#endif