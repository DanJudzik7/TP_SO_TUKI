#ifndef UTILS_CPU_H
#define UTILS_CPU_H

#include <math.h>

#include "shared_package.h"

extern configuration_cpu config_cpu;

// Obtiene la dirección de un registro en base a su nombre en texto
char* register_pointer(char* name, t_registers* registers);

// Obtiene el tamaño de un registro en base a su nombre en texto
int size_of_register_pointer(char* name, t_registers* registers);

// Traduce la dirección lógica a física
t_physical_address* mmu(int logical_address, int size, t_execution_context* ec);

// Obtener segmento por id del segmento
t_segment* list_get_by_sid(t_list* list, int id);

#endif