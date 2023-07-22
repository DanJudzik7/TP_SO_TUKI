#ifndef UTILS_MEMORIA_H
#define UTILS_MEMORIA_H

#include <pthread.h>

#include "memory_structures.h"
#include "shared_package.h"
#include "shared_socket.h"

void setup_config();

t_memory_structure* new_memory_structure(void* memory);

#endif