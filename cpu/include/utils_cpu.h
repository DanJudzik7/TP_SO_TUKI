#ifndef UTILS_CPU_H
#define UTILS_CPU_H

#include "shared_package.h"

extern configuration_cpu config_cpu;

// Obtiene la dirección de un registro en base a su nombre en texto
char* register_pointer(char* name, cpu_register* registers);

// Traduce la dirección lógica a física
t_physical_address* mmu(int logic_address, int size, execution_context* ec);

// Obtener segmento por id del segmento
segment* list_get_by_sid(t_list* list, int id);

#endif