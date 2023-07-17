#ifndef HANDLER_CPU_H
#define HANDLER_CPU_H

#include <math.h>

#include "shared_package.h"
#include "utils_cpu.h"

extern configuration_cpu config_cpu;

t_instruction* fetch(execution_context* execution_context);

t_physical_address* decode(t_instruction* instruction, execution_context* ec);

void execute(t_instruction* instruction, execution_context* ec, t_physical_address* associated_pa);

void set_register(char* register_name, char* value, cpu_register* registers);

void execute_exit(execution_context* execution_context);

void dislodge();

#endif