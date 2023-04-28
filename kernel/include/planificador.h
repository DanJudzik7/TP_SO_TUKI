#ifndef PLANIFICADOR_H
#define PLANIFICADOR_H


#include <stdio.h>
#include "algorithms.h"
#include "shared_utils.h"


pcb* planificador_kernel(t_log* logger,pcb *pcb, char* algorithm,t_queue* queue_global_pcb );

#endif