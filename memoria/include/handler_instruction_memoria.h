#ifndef HANDLER_INSTRUCTION_MEMORIA_H
#define HANDLER_INSTRUCTION_MEMORIA_H

#include "utils_memoria.h"
#include "algorithms.h"
#include "shared_serializer.h"
#include "shared_socket.h"

int instruction_handler_memoria(int socket_client);
// Crea una tabla de segmentos para un pid en especifico
t_list* create_sg_table(memory_structure* structures,int process_id); 
// Remueve una tabla de segmentos para un pid en especifico
void remove_sg_table(memory_structure* structures,int process_id);
// Addiere un segmento a una tabla de segmentos de un pid en especifico
void add_segment(memory_structure* structures,int process_id, int size,int s_id);

#endif