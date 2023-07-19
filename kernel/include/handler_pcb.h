#ifndef HANDLER_PCB_H
#define HANDLER_PCB_H

#include <pthread.h>

#include "scheduler.h"
#include "shared_serializer.h"
#include "shared_socket.h"

typedef struct t_pcb {
	int pid;  // También funciona como ID del socket
	process_state state;
	int aprox_burst_time;
	time_t last_ready_time;
	t_dictionary* files;
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
	int connection_kernel;
	int connection_cpu;
	t_pcb* prioritized_pcb;
	int socket_memory;
} t_global_config_kernel;

typedef struct helper_create_pcb {
	int connection;
	t_global_config_kernel* gck;
} helper_create_pcb;

typedef struct t_helper_file_instruction {
	int socket_filesystem;
	t_log* logger;
	t_queue* file_instructions;
} t_helper_file_instruction;

// Servidor para Consolas, que genera un PCB para cada una
void listen_consoles(t_global_config_kernel* gck);

// Administrador de conexiones de lectura y escritura a Filesystem
void handle_fs(t_helper_file_instruction* hfi);

// Inicializa un proceso en kernel y sus estructuras
void handle_new_process(helper_create_pcb* hcp);

// Termina un proceso en kernel y elimina sus estructuras
void exit_process(t_pcb* pcb, t_global_config_kernel* gck);

// Inicializa un PCB con los datos recibidos
t_pcb* pcb_new(int pid, int burst_time);

// Limpia un PCB de la memoria
void pcb_destroy(t_pcb* pcb);

// Limpia una t_instruction de la memoria
void instruction_delete(t_instruction* instruction);

#endif