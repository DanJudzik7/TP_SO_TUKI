#ifndef SHARED_UTILS_H
#define SHARED_UTILS_H

// Carga librerías varias de utilidades generales
#include <commons/collections/list.h>
#include <commons/collections/queue.h>
#include <commons/config.h>
#include <commons/log.h>
#include <commons/string.h>
#include <semaphore.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <unistd.h>

typedef struct t_package {
	uint64_t size;
	int32_t type;
	void* buffer;
} t_package;

typedef struct cpu_register_4 {
	char AX[4];
	char BX[4];
	char CX[4];
	char DX[4];
} cpu_register_4;

typedef struct cpu_register_8 {
	char EAX[8];
	char EBX[8];
	char ECX[8];
	char EDX[8];
} cpu_register_8;

typedef struct cpu_register_16 {
	char RAX[16];
	char RBX[16];
	char RCX[16];
	char RDX[16];
} cpu_register_16;

typedef struct cpu_register {
	cpu_register_4 register_4;
	cpu_register_8 register_8;
	cpu_register_16 register_16;
} cpu_register;

typedef struct segment_table {
	uint32_t id;
	void* segment_table_direction;
	uint8_t size_data_segment;
} segment_table;

typedef struct t_file {
	uint32_t size_file;
	void* file_direction;
} t_file;

typedef enum process_state {
	NEW,
	READY,
	EXEC,
	BLOCK,
	EXIT_PROCESS
} process_state;

typedef struct execution_context {
	t_queue* instructions;
	uint32_t program_counter;
	process_state updated_state;
	cpu_register* cpu_register;
	segment_table* segment_table;
} execution_context;

typedef enum execution_context_index {
	EC_INSTRUCTIONS,
	PROGRAM_COUNTER,
	UPDATED_STATE,
	CPU_REGISTERS,
	SEGMENT_TABLE
} execution_context_index;

typedef struct config_cpu {
	t_log* logger;
	sem_t flag_dislodge;
	sem_t flag_running;
	int connection_kernel;
} configuration_cpu;

typedef struct config_memory {
	t_log* logger;
	t_config* config;
	char* algorithm;
} configuration_memory;

// Carga la configuración de un módulo
t_config* start_config(char* module);

// Crea un logger para un módulo
t_log* start_logger(char* module);

// Retorna el valor de una key de un archivo de configuración
char* get_config_type(char* module, char* file_type);

// Safe Memory Allocation. Crashea si no hay más memoria.
void* s_malloc(size_t size);

void print_execution_context(execution_context* execution_context);

#endif