#ifndef MEMORY_STRUCTURES_H
#define MEMORY_STRUCTURES_H

#include "shared_serializer.h"
#include "shared_socket.h"

// ----------------------------- STRUCTS -----------------------------
// Estructura para cada agujero
typedef struct hole {
	void* base;
	int size;
} hole;

// Estructura para la configuración de memoria
typedef struct t_config_memory {
	t_log* logger;
	t_config* config;
	char* port;
	size_t memory_size;
	size_t sg_zero_size;
	int sg_amount;
	int mem_delay;
	int com_delay;
	char* algorithm;
	int remaining_memory;
} t_config_memory;



typedef struct t_memory_thread {
	int socket;
	t_memory_structure* mem_structure;
} t_memory_thread;

#endif