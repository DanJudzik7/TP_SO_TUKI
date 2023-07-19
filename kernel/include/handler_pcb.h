#ifndef HANDLER_PCB_H
#define HANDLER_PCB_H

#include <pthread.h>

#include "scheduler.h"
#include "shared_serializer.h"
#include "shared_socket.h"

typedef struct helper_create_pcb {
	int connection;
	t_global_config_kernel* gck;
} helper_create_pcb;

typedef struct t_helper_file_instruction {
	int socket_filesystem;
	t_log* logger;
	t_dictionary* global_files; // Key-value: filename - list of waiting pcb
	t_queue* file_instructions;
} t_helper_file_instruction;

// Servidor para Consolas, que genera un PCB para cada una
void listen_consoles(t_global_config_kernel* gck);

// Administrador de conexiones de lectura y escritura a Filesystem
void handle_fs(t_helper_file_instruction* hfi);

// Inicializa un proceso en kernel y sus estructuras
void handle_new_process(helper_create_pcb* hcp);

// Limpia una t_instruction de la memoria
void instruction_destroy(t_instruction* instruction);

#endif