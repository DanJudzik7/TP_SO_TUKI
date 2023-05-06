#ifndef HANDLER_INSTRUCTION_KERNEL_H
#define HANDLER_INSTRUCTION_KERNEL_H

#include <stdio.h>

#include "shared_utils.h"

// Administrador de instrucciones del kernel
int instruction_handler_kernel();

// Administrador de instrucciones recibidas por consola
void handle_incoming_instructions(t_pcb* pcb);

#endif