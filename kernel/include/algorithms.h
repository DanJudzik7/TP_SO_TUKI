#ifndef ALGORITHMS_H
#define ALGORITHMS_H


#include <stdio.h>
#include "shared_utils.h"

t_pcb* fifo(t_queue* queue_global_pcb, t_pcb* t_pcb);
t_pcb* hrrn(t_log* logger,t_queue* queue_global_pcb, t_pcb *t_pcb);

#endif