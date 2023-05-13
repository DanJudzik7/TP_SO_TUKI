#ifndef HANDLER_PCB_H
#define HANDLER_PCB_H

#include <pthread.h>
#include <stdio.h>

#include "handler_instruction_kernel.h"
#include "shared_utils.h"
#include "planificador.h"

// Servidor para Consolas, que genera un PCB para cada una
void listen_consoles(t_global_config_kernel* gck);

// Crea un PCB con los datos recibidos
t_pcb* pcb_new(int pid, int burst_time);

// Limpia un PCB de la memoria
void pcb_destroy(t_pcb* pcb);

// Limpia un t_file de la memoria
void delete_file(t_file* file);

// Limpia una t_instruction de la memoria
void delete_instruction(t_instruction* instruction);

#endif