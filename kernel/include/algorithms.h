#ifndef ALGORITHMS_H
#define ALGORITHMS_H


#include <stdio.h>
#include "shared_utils.h"

pcb* fifo(t_queue* queue_global_pcb, pcb *pcb);
pcb* hrrn(t_log* logger,t_queue* queue_global_pcb, pcb *pcb);

#endif