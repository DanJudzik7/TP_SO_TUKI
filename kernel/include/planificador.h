#ifndef PLANIFICADOR_KERNEL_H
#define PLANIFICADOR_KERNEL_H


#include <stdio.h>
#include "shared_utils.h"


pcb* planificador(pcb new_pcb, char* algorithm,t_queue* queue_global_pcb );

#endif