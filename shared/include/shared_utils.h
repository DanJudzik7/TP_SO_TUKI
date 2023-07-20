#ifndef SHARED_UTILS_H
#define SHARED_UTILS_H

// Carga librerías varias de utilidades generales
#include <commons/bitarray.h>
#include <commons/collections/dictionary.h>
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

typedef struct t_registers {
	// Registros de 4 bytes
	char AX[4];
	char BX[4];
	char CX[4];
	char DX[4];
	// Registros de 8 bytes
	char EAX[8];
	char EBX[8];
	char ECX[8];
	char EDX[8];
	// Registros de 16 bytes
	char RAX[16];
	char RBX[16];
	char RCX[16];
	char RDX[16];
} t_registers;

// Estructura de cada segmento
typedef struct t_segment {
	void* base;
	int offset;
	int s_id;
} t_segment;

// Dirección Física
typedef struct t_physical_address {
	int segment;
	int offset;
} t_physical_address;

typedef enum t_memory_op {
	MEM_INIT_PROCESS, // 0
	MEM_END_PROCCESS, // 1
	MEM_READ_ADDRESS, // 2
	MEM_WRITE_ADDRESS, // 3
	MEM_CREATE_SEGMENT, // 4
	MEM_DELETE_SEGMENT,	// 5
	MEM_COMPACT_ALL, // 6
} t_memory_op;

typedef enum t_memory_state {
	SEG_FAULT,		  // 0
	OK_INSTRUCTION,	  // 1
	NO_SPACE_LEFT,	  // 2
	COMPACT_REQUEST,  // 3
	SEGMENT,		  // 4
} t_memory_state;

typedef enum process_state {
	NEW,
	READY,
	EXEC,
	BLOCK,
	EXIT_PROCESS
} process_state;

// Estructuras para pasar solo este struct y no cada una de manera individual
typedef struct t_memory_structure {
	t_segment* segment_zero;
	t_dictionary* table_pid_segments;  // un diccionario con todas las tablas de segmentos de todos los procesos  [ key: [lista_segmentos ] ]
	t_list* hole_list;
	t_list* ram;  // La memoria en si
	void* heap;
} t_memory_structure;

typedef struct t_instruction {
	int32_t op_code;
	t_list* args;
} t_instruction;

typedef struct t_execution_context {
	t_queue* instructions;
	uint32_t program_counter;
	t_registers* cpu_register;
	t_list* segments_table;
	uint32_t pid;
	t_instruction* kernel_request;	// Acá la CPU va a guardar si necesita algo de kernel
} t_execution_context;

typedef enum execution_context_index {
	EC_INSTRUCTIONS,
	PROGRAM_COUNTER,
	PROCESS_PID,
	CPU_REGISTERS,
	SEGMENTS_TABLE,
	ALL_SEGMENTS_TABLES,
	F_WRITE_READ,
	KERNEL_REQUEST
} execution_context_index;

typedef struct config_cpu {
	t_log* logger;
	bool flag_dislodge;
	int socket_memory;
	int max_segment_size;
	int instruction_delay;
} configuration_cpu;

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
	YIELD,			 // 15
} op_code;

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

void print_execution_context(t_execution_context* execution_context);

// Retorna si un valor está en una lista
bool is_in_list(t_list* list, char* value);

// Crea una nueva instrucción con el tipo proporcionado
t_instruction* instruction_new(int32_t op_code);

// Duplica una instrucción
t_instruction* instruction_duplicate(t_instruction* source);

// Elimina una instrucción
void instruction_destroy(t_instruction* instruction);

// Inicializa un Execution Context con los datos recibidos
t_execution_context* execution_context_new(int pid);

// Limpia un Execution Context de la memoria
void execution_context_destroy(t_execution_context* ec);

#endif