#ifndef UTILS_MEMORIA_H
#define UTILS_MEMORIA_H

#include "handler_instruction_memoria.h"
#include "shared_package.h"
#include "shared_socket.h"
#include "tests.h"
#include <pthread.h>


// ----------------------------- STRUCTS -----------------------------

// Estructura de cada segmento
typedef struct segment{
	void* base;
	int offset;
	int s_id;
} segment;

// Estructura para cada agujero
typedef struct hole{
	void* base;
	int size;
} hole;

// Estructura para la configuracion de memoria
typedef struct memory {
	size_t memory_size;
    size_t sg_zero_size;
    int sg_amount;
    int mem_delay;
    int com_delay;
    char* algorithm; 
	int remaining_memory;
} memory;

// Estructuras para pasar solo este struct y no cada una de manera individual
typedef struct structures{
	segment* segment_zero;
	t_dictionary* all_segments;
	t_list* hole_list;
	t_list* ram; // La memoria en si
} structures;


extern configuration_memory memory_config;
extern memory memory_shared;
// ----------------------------- HEADERS -----------------------------

void initialize();
t_list* create_sg_table(structures structures,char* process_id);
void remove_sg_table(structures structures,char* process_id);
void add_segment(structures structures,char* process_id, int size,int s_id);
segment* first_fit(structures structures, int size, char* pid, int s_id);
bool is_hole_empty(hole* h);
segment* best_fit(structures structures, int size, char* pid, int s_id);
segment* worst_fit(structures structures, int size, char* pid, int s_id);



#endif