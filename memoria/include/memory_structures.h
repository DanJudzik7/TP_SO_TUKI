#ifndef MEMORY_STRUCTURES_H
#define MEMORY_STRUCTURES_H

#include "shared_serializer.h"
#include "shared_socket.h"

// ----------------------------- STRUCTS -----------------------------
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
typedef struct memory_structure{
	segment* segment_zero;
	t_dictionary* table_pid_segments;  //un diccionario con todas las tablas de segmentos de todos los procesos  [ key: [lista_segmentos ] ]
	t_list* hole_list;
	t_list* ram; // La memoria en si  
} memory_structure;

typedef struct thread{
	int socket;
	memory_structure* mem_structure;
} thread;


#endif