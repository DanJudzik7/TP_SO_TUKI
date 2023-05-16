#ifndef HANDLER_PCB_H
#define HANDLER_PCB_H

#include <pthread.h>

#include "shared_serializer.h"
#include "shared_socket.h"

typedef struct t_global_config_kernel {
	t_log* logger;
	t_queue* new_pcbs;
	t_queue* active_pcbs;
	bool algorithm_is_hrrn;
	int max_multiprogramming;
	int default_burst_time;
	int connection_kernel;
} t_global_config_kernel;

typedef struct t_pcb {
	int pid;  // También funciona como ID del socket
	process_state state;
	int aprox_burst_time;
	time_t last_ready_time;
	t_list* files;
	execution_context* execution_context;
} t_pcb;


// Servidor para Consolas, que genera un PCB para cada una
void listen_consoles(t_global_config_kernel* gck);

// Administrador de instrucciones recibidas por consola
void handle_incoming_instructions(t_pcb* pcb);

// Crea un PCB con los datos recibidos
t_pcb* pcb_new(int pid, int burst_time);

// Limpia un PCB de la memoria
void pcb_destroy(t_pcb* pcb);

// Limpia un t_file de la memoria
void delete_file(t_file* file);

// Limpia una t_instruction de la memoria
void delete_instruction(t_instruction* instruction);

#endif