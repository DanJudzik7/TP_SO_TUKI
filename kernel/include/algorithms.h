#ifndef ALGORITHMS_H
#define ALGORITHMS_H


#include <stdio.h>
#include "shared_utils.h"

t_pcb* pick_with_fifo(t_queue* queue_global_pcb);
bool pcb_is_ready(t_pcb* pcb);
t_pcb* pick_with_hrrn(t_queue* queue_global_pcb);

#endif