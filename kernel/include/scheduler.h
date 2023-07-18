#ifndef SCHEDULER_H
#define SCHEDULER_H

#include "algorithms_kernel.h"
#include "handler_pcb.h"
#include "shared_utils.h"

// Se llama cuando: se crean procesos, o cuando uno existente llega a EXIT
void long_term_schedule(t_global_config_kernel* gck);

// Organiza a Corto Plazo, devolviendo el próximo PCB a ejecutar en base al algoritmo y los PCBs activos.
t_pcb* short_term_scheduler(t_global_config_kernel* gck);

#endif