#ifndef SHARED_SERIALIZER_H
#define SHARED_SERIALIZER_H

#include "shared_utils.h"
#include "shared_package.h"

// Librería de serialización
// Se encarga de convertir varios tipos de datos distintos a t_package y viceversa

// Genera un paquete de modo mensaje
t_package* serialize_message(char* value, bool is_flaw);

// Devuelve el mensaje deserializado, y destruye el paquete
char* deserialize_message(t_package* package);

// Serializa el execution_context a un paquete
t_package* serialize_execution_context(t_execution_context* ec);

// Deserializa un Execution Context desde un paquete
t_execution_context* deserialize_execution_context(t_package* package);

// Serializa una lista de instrucciones
t_package* serialize_instructions(t_queue* instructions, bool is_ec);

// Serializa una instrucción
t_package* serialize_instruction(t_instruction* instruction);

// Deserializa las instrucciones y las agrega a una queue
void deserialize_instructions(t_package* package, t_queue* instructions);

// Deserializa una instrucción
t_instruction* deserialize_instruction(t_package* package);

// Serializa los registros de la CPU
t_package* serialize_cpu_registers(cpu_register* registers);

// Deserializa los registros de la CPU
cpu_register* deserialize_cpu_registers(void* source);

// Serializa todos las tablas de segmentos en memoria
t_package* serialize_all_segments_tables(t_memory_structure* mem_struct);

// Serializa una tabla de segmentos
t_package* serialize_segments_table(t_list* segment_table, uint32_t type, void* heap_pointer);

// Serializa un segmento
t_package* serialize_segment(t_segment* segment, void* heap_pointer);

// Deserializa una tabla de segmentos
t_list* deserialize_segment_table(t_package* package);

// Deserializa un segmento
t_segment* deserialize_segment(t_package* nested);

// Deserializa todos los segmentos
t_dictionary* deserialize_all_segments_tables(t_package* package);

#endif