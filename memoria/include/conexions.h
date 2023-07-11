#ifndef CONEXIONS_H
#define CONEXIONS_H

#include "utils_memoria.h"
#include "handlers.h"

void listen_modules(int socket_memory,memory_structure* structures);
int listen_kernel(int socket_memory);
int listen_cpu(int socket_memory);
int listen_fs(int socket_memory);

#endif