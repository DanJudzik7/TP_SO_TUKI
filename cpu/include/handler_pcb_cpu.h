#ifndef HANDLER_PCB_CPU_H
#define HANDLER_PCB_CPU_H


#include <stdio.h>
#include "shared_utils.h"

void execute(execution_context* execution_context, int pid);

void set(execution_context* execution_context);
#endif