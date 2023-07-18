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
t_package* serialize_execution_context(execution_context* ec);

// Deserializa un Execution Context desde un paquete
execution_context* deserialize_execution_context(t_package* package);

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

// Serializa una tabla de segmentos
t_package* serialize_segment_table(t_memory_structure* mem_struct, t_list* segment_table,int pid);

// Serializa un segmento
t_package* serialize_segment(segment* segment,t_memory_structure* mem_struct,int pid);

// Serializa todos las tablas de segmentos en memoria
t_package* serialize_all_segments(t_memory_structure* mem_struct);

// Deserializa una tabla de segmentos
t_list* deserialize_segment_table(t_package* package);

// Deserializa un segmento
segment* deserialize_segment(t_package* nested);

// Deserializa todos los segmentos
t_dictionary* deserialize_all_segments(t_package* package);

/* // Serializa un segmento de f_write y f_read
t_package* serialize_segment_read_write(segment_read_write* seg_rw);

// Derializa un segmento de f_write y f_read
segment_read_write* deserialize_segment_read_write(void* source); */

// Serializa un program counter
t_package* serialize_program_counter(uint32_t *program_counter, uint64_t* value_size);

// Deserializa un program counter
uint32_t deserialize_program_counter(void* buffer, void* dest, uint64_t* offset);

// Serializa el paquete especificado reemplazando al original
void serialize_package(t_package* package);

//Deserializo el buffer de memoria
memory_buffer* deserialize_memory_buffer(t_package* package);
//Serializo el buffer de memoria
t_package* serialize_memory_buffer(uint32_t pid ,char* buffer);

#endif