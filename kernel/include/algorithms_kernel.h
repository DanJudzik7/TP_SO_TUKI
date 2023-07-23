#ifndef ALGORITHMS_KERNEL_H
#define ALGORITHMS_KERNEL_H

#include "shared_utils.h"
#include "kernel_utils.h"

// Planifica a Corto Plazo con FIFO, devolviendo el próximo PCB a ejecutar en base al algoritmo y los PCBs activos.
t_pcb* pick_with_fifo(t_queue* queue_global_pcb);

// Devuelve true si el PCB está en estado READY
bool pcb_is_ready(t_pcb* pcb);

// Planifica a Corto Plazo con HRRN, devolviendo el próximo PCB a ejecutar en base al algoritmo y los PCBs activos.
t_pcb* pick_with_hrrn(t_queue* queue_global_pcb);

#endif