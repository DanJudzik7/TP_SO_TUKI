#ifndef KERNEL_UTILS_H
#define KERNEL_UTILS_H

#include "handler_pcb.h"
#include "kernel_utils.h"
#include "scheduler.h"
#include "shared_serializer.h"
#include "shared_socket.h"
#include "shared_utils.h"

typedef struct resources_table {
	int instances;
	t_queue* resource_queue;
} resources_table;

t_global_config_kernel* new_global_config_kernel(t_config* config);

void exit_process(t_pcb* pcb, t_global_config_kernel *gck);

#endif