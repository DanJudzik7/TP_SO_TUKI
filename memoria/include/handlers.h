#ifndef HANDLERS_H
#define HANDLERS_H

#include "utils_memoria.h"
#include "conexions.h"

void handle_fs(int socket_fs);
void handle_cpu(int socket_cpu);
void handle_kernel(int socket_kernel);

#endif