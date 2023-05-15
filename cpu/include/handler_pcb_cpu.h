#ifndef HANDLER_PCB_CPU_H
#define HANDLER_PCB_CPU_H

#include <stdio.h>

#include "cpu.h"
#include "handler_execute_instructions.h"
#include "shared_utils.h"

extern configuration_cpu config_cpu;

void fetch(execution_context* execution_context);
execution_context* decode(execution_context* execution_context, t_list* instruction);
t_list* get_instruction(execution_context* execution_context);
void dislodge();

#endif