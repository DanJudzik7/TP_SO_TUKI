#ifndef UTILS_MEMORIA_H
#define UTILS_MEMORIA_H

#include "memory_structures.h"
#include "shared_package.h"
#include "shared_socket.h"
#include "tests.h"
#include <pthread.h>

extern configuration_memory memory_config;
extern memory memory_shared;
// ----------------------------- HEADERS -----------------------------
// Para proposito de verificar funcionamiento(debug)
void segmento_hardcodeado(int PID, int SEGMENTO, memory_structure* memory_structure);

void initialize();
// Función para graficar la RAM
void graph_ram(memory_structure* memory_structure, void* memory_base);
// Función para graficar una tabla especifica de tipo table_pid_segments
void graph_specific_table_pid_segments(t_list* segment_table, int process_id, void* memory_base);
// Función para graficar la tabla table_pid_segments
void graph_table_pid_segments(t_dictionary* table_pid_segments, void* memory_base);
// Función para graficar la RAM y la tabla table_pid_segments
void graph_memory(memory_structure* memory_struct, void* memory_base) ;
// Transforma la base dada a un entero para saber su posicion en la ram
uint32_t transform_base_to_decimal(void*  address, void* memory_base);

#endif