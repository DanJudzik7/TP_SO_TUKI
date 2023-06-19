#ifndef CONEXIONS_H
#define CONEXIONS_H

#include "utils_memoria.h"


void listen_modules(int socket_memory);
void listen_kernel(int socket_memory);
void listen_cpu(int socket_memory);
void listen_fs(int socket_memory);
#endif