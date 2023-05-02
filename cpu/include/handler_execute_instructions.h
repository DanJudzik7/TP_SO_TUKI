#ifndef HANDLER_EXECUTE_INSTRUCTIONS_H
#define HANDLER_EXECUTE_INSTRUCTIONS_H

#include <stdio.h>
#include "shared_utils.h"

void set(execution_context* execution_context, t_list* instruction);

void yield(t_pcb* t_pcb);

// Les renombré a exitIns porque me hacía conflicto con la función exit nativa
void exitIns(t_pcb* t_pcb);

void dislodge(t_pcb* t_pcb);

#endif