#ifndef HANDLER_PCB_CPU_H
#define HANDLER_PCB_CPU_H

#include "handler_execute_instructions.h"
#include "shared_serializer.h"
#include "shared_socket.h"
#include "shared_package.h"
#include <math.h>


typedef struct segment{
	void* base;
	int offset;
	int s_id;
} segment;

extern configuration_cpu config_cpu;

void fetch(execution_context* execution_context);

execution_context* decode(execution_context* execution_context, t_instruction* instruction);

t_instruction* get_instruction(execution_context* execution_context);

void dislodge();

// MMU, traduccion de DL a DF
t_physical_address* MMU(int logic_address, int size, execution_context* ec);

// Obtener segmento por id del segmento
segment* list_get_by_sid(t_list* list, int id);

void print_instruction(t_instruction* instruction);

#endif