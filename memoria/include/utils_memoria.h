#ifndef UTILS_MEMORIA_H
#define UTILS_MEMORIA_H

#include "handler_instruction_memoria.h"
#include "shared_package.h"
#include "shared_socket.h"
#include "tests.h"
#include <pthread.h>

// ----------------------------- HEADERS -----------------------------

void initialize();


// ----------------------------- STRUCTS -----------------------------

typedef struct segment{
	void* base;
	int offset;
	int pid;
	int is_in_use;
} segment;

typedef struct hole{
	void* base;
	int size;
} hole;

typedef struct memory {
	size_t memory_size;
    size_t sg_zero_size;
    int sg_amount;
    int mem_delay;
    int com_delay;
    char* algorithm;
    
} memory;

extern configuration_memory memory_config;
extern memory memory_shared;



#endif