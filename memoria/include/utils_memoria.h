#ifndef UTILS_MEMORIA_H
#define UTILS_MEMORIA_H

#include "handler_instruction_memoria.h"
#include "shared_package.h"
#include "shared_socket.h"
#include "tests.h"
#include <pthread.h>


// ----------------------------- STRUCTS -----------------------------

typedef struct segment{
	void* base;
	int offset;
	int s_id;
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

typedef struct structures{
	segment* segment_zero;
	t_dictionary* all_segments;
	t_list* hole_list;
} structures;


extern configuration_memory memory_config;
extern memory memory_shared;
// ----------------------------- HEADERS -----------------------------

void initialize();
void create_structure(structures structures);
void remove_structure(structures structures);



#endif