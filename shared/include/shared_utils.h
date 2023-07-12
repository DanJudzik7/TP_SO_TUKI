#ifndef SHARED_UTILS_H
#define SHARED_UTILS_H

// Carga librerías varias de utilidades generales
#include <commons/collections/list.h>
#include <commons/collections/queue.h>
#include <commons/collections/dictionary.h>
#include <commons/config.h>
#include <commons/log.h>
#include <commons/string.h>
#include <commons/bitarray.h>
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

typedef struct instruction_memory{
	uint32_t pid;
	uint32_t s_id;
	uint32_t offset;
} instruction_memory;

typedef struct memory_buffer{
	uint32_t pid;
	char* buffer;
} memory_buffer;

typedef struct segment_table {
	uint32_t pid;
	uint32_t s_id;
	uint32_t size_segment;
	t_list* segment_table_pcb;
} segment_table;

// Estructura de cada segmento
typedef struct segment{
	void* base;
	int offset;
	int s_id;
} segment;

//Direccion fisica
typedef struct physical_address {
    int segment;
    int offset;
} t_physical_address;

typedef struct segment_read_write {
	uint32_t s_id;
    uint32_t offset;
    uint32_t size;
    char* buffer;
    uint32_t pid;
} segment_read_write;

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
	t_list* segment_table;
	uint32_t pid;
} execution_context;

typedef enum execution_context_index {
	EC_INSTRUCTIONS,
	PROGRAM_COUNTER,
	UPDATED_STATE,
	CPU_REGISTERS,
	SEGMENT_TABLE,
	F_WRITE_READ,
} execution_context_index;

typedef struct config_cpu {
	t_log* logger;
	int connection_kernel;
	int max_segment_size;
} configuration_cpu;

typedef struct config_filesystem {
	t_log* logger;
	t_config* config;
	t_bitarray* bitmap;
	char* block_file;
	int connection_kernel;
	int socket_memoria;
	int block_size;
	int block_count;
	char* PATH_SUPERBLOQUE;
	char* PATH_BITMAP;
	char* PATH_BLOQUES;
	char* PATH_FCB;
} configuration_filesystem;

typedef struct config_memory {
	t_log* logger;
	t_config* config;
	char* port;
} configuration_memory;

typedef enum op_code {
	F_READ,			 // 0
	F_WRITE,		 // 1
	SET,			 // 2
	MOV_IN,			 // 3
	MOV_OUT,		 // 4
	F_TRUNCATE,		 // 5
	F_SEEK,			 // 6
	CREATE_SEGMENT,	 // 7
	I_O,			 // 8
	WAIT,			 // 9
	SIGNAL,			 // 10
	F_OPEN,			 // 11
	F_CLOSE,		 // 12
	DELETE_SEGMENT,	 // 13
	EXIT,			 // 14
	YIELD,		 // 15
} op_code;

typedef struct t_instruction {
	op_code op_code;
	t_list* args;
} t_instruction;

// Carga la configuración de un módulo
t_config* start_config(char* module);

// Crea un logger para un módulo
t_log* start_logger(char* module);

// Retorna el valor de una key de un archivo de configuración
char* get_config_type(char* module, char* file_type);

// Retorna el valor del path completo a un archivo
char* get_full_path(char* path);

// Safe Memory Allocation. Crashea si no hay más memoria.
void* s_malloc(size_t size);

void print_execution_context(execution_context* execution_context);

void print_instruction(t_instruction* instruction);

#endif