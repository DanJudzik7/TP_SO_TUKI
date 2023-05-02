#ifndef PLANIFICADOR_H
#define PLANIFICADOR_H


#include <stdio.h>
#include "algorithms.h"
#include "shared_utils.h"


void planificador_kernel(t_console_init* ci);
op_code_reception* long_term_scheduler(t_console_init* ci);

#endif