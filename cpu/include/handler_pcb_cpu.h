#ifndef HANDLER_PCB_CPU_H
#define HANDLER_PCB_CPU_H

#include "handler_execute_instructions.h"
#include "shared_serializer.h"
#include "shared_socket.h"
#include "shared_package.h"

extern configuration_cpu config_cpu;

void fetch(execution_context* execution_context);

execution_context* decode(execution_context* execution_context, t_instruction* instruction);

t_instruction* get_instruction(execution_context* execution_context);

void dislodge();

#endif