#ifndef HANDLER_INSTRUCTION_KERNEL_H
#define HANDLER_INSTRUCTION_KERNEL_H


#include <stdio.h>
#include "shared_utils.h"

// Manejador de instrucciones del kernel
int instruction_handler_kernel();

void iterator(char* value);
//Manejador de instrucciones recibidas por consola
t_list* instruction_handler_reciver(int cliente_fd, t_log* logger);

int instruction_handler_reciver_not_funcional(int cliente_fd);


void list_instructions ( char* instructions );

#endif