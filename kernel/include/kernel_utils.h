#ifndef KERNEL_UTILS_H
#define KERNEL_UTILS_H

#include "shared_serializer.h"
#include "shared_socket.h"

typedef struct t_pcb {
	uint32_t pid;  // También funciona como ID del socket
	process_state state;
	int aprox_burst_time;
	time_t last_ready_time;
	t_dictionary* local_files; // Key-value: filename - position
	t_execution_context* execution_context;
} t_pcb;

typedef struct t_global_config_kernel {
	t_log* logger;
	t_queue* new_pcbs;
	t_queue* active_pcbs;
	t_dictionary* resources;
	bool algorithm_is_hrrn;
	int max_multiprogramming;
	int default_burst_time;
	int server_socket;
	int connection_cpu;
	t_pcb* prioritized_pcb;
	int socket_memory;
} t_global_config_kernel;

typedef struct t_resource {
	int available_instances;
	t_pcb* assigned_to;
	t_queue* enqueued_processes;
} t_resource;

typedef struct t_helper_pcb_io {
	t_pcb* pcb;
	t_log* logger;
	int time;
} t_helper_pcb_io;

t_global_config_kernel* new_global_config_kernel(t_config* config);

void handle_pcb_io(t_helper_pcb_io* hpi);

t_resource* resource_get(t_pcb* pcb, t_global_config_kernel* gck, char* name);

// Libera un recursos y lo asigna al siguiente proceso en queue
void resource_signal(t_resource* resource, t_log* logger);

// Inicializa un PCB con los datos recibidos
t_pcb* pcb_new(int pid, int burst_time);

// Limpia un PCB de la memoria
void pcb_destroy(t_pcb* pcb);

#endif