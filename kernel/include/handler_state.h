#ifndef HANDLER_STATE_H
#define HANDLER_STATE_H

#include "handler_pcb.h"
#include "kernel_utils.h"
#include "scheduler.h"
#include "shared_serializer.h"
#include "shared_utils.h"
#include "shared_socket.h"

void state_exit_process(t_pcb* pcb, t_global_config_kernel *gck);

void state_signal_or_wait(t_pcb* pcb, t_global_config_kernel *gck);

void state_yield(t_pcb* pcb, t_global_config_kernel *gck);

#endif