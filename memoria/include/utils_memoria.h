#ifndef UTILS_MEMORIA_H
#define UTILS_MEMORIA_H

#include <pthread.h>

#include "memory_structures.h"
#include "shared_package.h"
#include "shared_socket.h"

// Carga configuraciones y otros
void setup_config(char* config_path);

// Inicializa Memory Structure
t_memory_structure* new_memory_structure(void* memory);

// Transforma la base dada a un entero para saber su posición en la ram
uint32_t transform_base_to_decimal(void* address, void* memory_base);
#endif