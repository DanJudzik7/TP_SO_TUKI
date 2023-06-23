#ifndef MEMORIA_H
#define MEMORIA_H

#include "utils_memoria.h"
#include "conexions.h"
#include "handler_instruction_memoria.h"

void createSGZero(void* memory, segment* segmentZero);

void segmento_hardcodeado(int PID, int SEGMENTO, memory_structure* memory_structure);

#endif