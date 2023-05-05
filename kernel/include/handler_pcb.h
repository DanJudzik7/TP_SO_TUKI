#ifndef HANDLER_PCB_H
#define HANDLER_PCB_H

#include <pthread.h>
#include <stdio.h>

#include "handler_instruction_kernel.h"
#include "shared_utils.h"
#include "planificador.h"

// Genera un nuevo PCB desde una nueva consola
void listen_console(global_config_kernel* gck);
void listen_cpu(config_current_process* config_current_process);
t_pcb* create_pcb(process* process, t_log* logger,int default_burst_time);

int aprox_burst_time(int value_aprox_time);

#endif