#ifndef HANDLERS_H
#define HANDLERS_H

#include "utils_memoria.h"
#include "conexions.h"
#include "handler_instruction_memoria.h"

typedef struct fs_package{
       char* name;
       uint32_t pid;
       uint32_t size;
       //[physical_address]
       uint32_t s_id;
       uint32_t offset;
} fs_package;	

void handle_fs(int socket_fs,memory_structure* structures);
void handle_cpu(int socket_cpu,memory_structure* structures);
void handle_kernel(int socket_kernel,memory_structure* structures);
#endif