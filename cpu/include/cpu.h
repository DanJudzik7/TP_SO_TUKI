#ifndef CPU_H
#define CPU_H

#include <pthread.h>

#include "handler_pcb_cpu.h"
#include "shared_utils.h"
#include "shared_socket.h"
#include "shared_serializer.h"
#include "tests.h"

extern configuration_cpu config_cpu;

void listen_kernel(int socket_cpu);
void listen_kernel(int socket_cpu);

execution_context* create_context_test();

#endif