#ifndef KERNEL_UTILS_H
#define KERNEL_UTILS_H

#include "handler_pcb.h"
#include "kernel_utils.h"
#include "scheduler.h"
#include "shared_serializer.h"
#include "shared_utils.h"
#include "shared_socket.h"

typedef struct resources_table{
       int instances;
       t_queue* resource_queue;
} resources_table;

bool no_more_instructions(execution_context* ec);

void resources_handler(t_pcb* pcb, process_state process_state, t_global_config_kernel* gck);

char* get_resource_name(t_pcb* pcb);

#endif