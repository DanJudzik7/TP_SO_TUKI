#ifndef MEMORIA_H
#define MEMORIA_H

#include "create_memory.h"
#include "delete_memory.h"
#include "handler_instruction_memoria.h"
#include "shared_package.h"
#include "shared_socket.h"
#include "tests.h"

typedef struct empty_space {
	void* base;
	size_t size;
	struct empty_space* next_empty_space;
} empty_space;
typedef struct segment_table_process {
	void* base;
	size_t size;
	int process_pid;
} segment_table_process;

typedef struct auxiliary_structure {
	segment_table_process* segment_table_process;
	empty_space* empty_space;
} auxiliary_structure;

typedef struct memory {
	void* segment_zero;
	size_t size;
} memory;

extern configuration_memory config_memory;
extern memory memory_shared;

execution_context* create_context_test();  // LUEGO BORRAR, TEST QUE NO VA ACA

#endif