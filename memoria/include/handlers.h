#ifndef HANDLERS_H
#define HANDLERS_H

#include <unistd.h>
#include "handler_instruction_memoria.h"

void listen_modules(int server_memory, t_memory_structure* structures);

void handle_modules(t_memory_thread* thread);

#endif