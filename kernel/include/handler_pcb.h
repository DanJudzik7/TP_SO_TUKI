#ifndef HANDLER_PCB_H
#define HANDLER_PCB_H

#include <pthread.h>
#include <stdio.h>

#include "handler_instruction_kernel.h"
#include "shared_utils.h"

// Genera un nuevo PCB desde una nueva consola
void create_pcb(t_console_init* ci);

int aprox_burst_time(int value_aprox_time);

#endif