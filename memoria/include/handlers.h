#ifndef HANDLERS_H
#define HANDLERS_H

#include "utils_memoria.h"
#include "conexions.h"

void handle_fs(int socket_fs,memory_structure* structures);
void handle_cpu(int socket_cpu,memory_structure* structures);
void handle_kernel(int socket_kernel,memory_structure* structures);

#endif