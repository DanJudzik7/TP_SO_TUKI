#ifndef MEMORIA_H
#define MEMORIA_H

#include "utils_memoria.h"
#include "conexions.h"
#include "handler_instruction_memoria.h"

void createSGZero(void* memory, segment* segmentZero);

void segmento_hardcodeado(int PID, int SEGMENTO, memory_structure* memory_structure);
//Test donde creo segmentos, los elimino, y se compactan los contiguos
void test_create_del_segm(memory_structure* memory_structure, void* memory_base);
//Test de compactacion de la mem principal
void test_compact(memory_structure* memory_structure, void* memory_base);
//Test de lectura y escritura
void test_rw(memory_structure* memory_structure, void* memory_base);
//Test de instrucciones a memoria
void test_instruction_memory();
//Test de buffer de memoria
void test_buffer_memory();

#endif