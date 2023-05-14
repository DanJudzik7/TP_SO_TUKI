#ifndef CPU_H
#define CPU_H
#include <commons/log.h>
#include <stdbool.h>
#include <stdio.h>

#include "handler_pcb_cpu.h"
#include "shared_utils.h"
#include "tests.h"

extern configuration_cpu config_cpu;

void listen_kernel(int socket_cpu);
void listen_kernel(int socket_cpu);

execution_context* create_context_test();

#endif