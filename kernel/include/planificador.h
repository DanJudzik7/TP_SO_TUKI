#ifndef PLANIFICADOR_H
#define PLANIFICADOR_H


#include <stdio.h>
#include "algorithms.h"
#include "shared_utils.h"

void planificador_kernel(process* process);
void reciver_new_pcb(config_current_process* current_config_process);
op_code_reception* long_term_scheduler(global_config_kernel* gck, process* process);
void short_term_scheduler(global_config_kernel* gck, process* process);

#endif