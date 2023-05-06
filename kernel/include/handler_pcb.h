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
t_pcb* create_pcb(int pid, int burst_time);

#endif