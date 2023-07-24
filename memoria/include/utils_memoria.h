#ifndef UTILS_MEMORIA_H
#define UTILS_MEMORIA_H

#include <pthread.h>

#include "memory_structures.h"
#include "shared_package.h"
#include "shared_socket.h"

void setup_config();

t_memory_structure* new_memory_structure(void* memory);
void graph_specific_table_pid_segments(t_list* segment_table, int process_id, void* memory_base);
void graph_table_pid_segments(t_dictionary* table_pid_segments, void* memory_base);

#endif