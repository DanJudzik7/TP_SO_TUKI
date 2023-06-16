#ifndef UTILS_MEMORIA_H
#define UTILS_MEMORIA_H

#include "handler_instruction_memoria.h"
#include "shared_package.h"
#include "shared_socket.h"
#include "tests.h"



void initialize();

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
	size_t memory_size;
    size_t sg_zero_size;
    int sg_amount;
    int mem_delay;
    int com_delay;
    char* algorithm;
    
} memory;

extern configuration_memory config_memory;
extern memory memory_shared;



#endif