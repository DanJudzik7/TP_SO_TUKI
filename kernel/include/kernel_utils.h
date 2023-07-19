#ifndef KERNEL_UTILS_H
#define KERNEL_UTILS_H

#include "handler_pcb.h"
#include "kernel_utils.h"
#include "scheduler.h"
#include "shared_serializer.h"
#include "shared_socket.h"
#include "shared_utils.h"

typedef struct t_resource {
	int available_instances;
	t_queue* enqueued_processes;
} t_resource;

typedef struct t_helper_pcb_io {
	t_pcb* pcb;
	int time;
} t_helper_pcb_io;

t_global_config_kernel* new_global_config_kernel(t_config* config);

void handle_pcb_io(t_helper_pcb_io* hpi);

t_resource* resource_get(t_pcb* pcb, t_global_config_kernel* gck, char* name);

#endif