#ifndef TESTS_MEMORIA_H
#define TESTS_MEMORIA_H

#include "memoria.h"

int main();

// Crea un segmento
void express_segment(int PID, int SEGMENTO, t_memory_structure* memory_structure);
// Test donde creo segmentos, los elimino, y se compactan los contiguos
void test_segment_creation();
// Test de compactacion de la mem principal
void test_compact();
// Test de lectura y escritura
void test_rw();
// Test de instrucciones a memoria
void test_instruction_memory();
// Test de buffer de memoria
void test_buffer();

// Función para graficar la RAM
void graph_ram(t_memory_structure* memory_structure, void* memory_base);
// Función para graficar una tabla especifica de tipo table_pid_segments
void graph_specific_table_pid_segments(t_list* segment_table, int process_id, void* memory_base);
// Función para graficar la tabla table_pid_segments
void graph_table_pid_segments(t_dictionary* table_pid_segments, void* memory_base);
// Función para graficar la RAM y la tabla table_pid_segments
void graph_memory(t_memory_structure* memory_struct, void* memory_base);

#endif