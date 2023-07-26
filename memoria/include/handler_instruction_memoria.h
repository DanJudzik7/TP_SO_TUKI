#ifndef HANDLER_INSTRUCTION_MEMORIA_H
#define HANDLER_INSTRUCTION_MEMORIA_H

#include "algorithms.h"
#include "shared_serializer.h"
#include "shared_socket.h"
#include "utils_memoria.h"

typedef struct struc_helper_delete {
	int s_id_to_delete;
	t_segment* segment;
} struc_helper_delete;

// Crea la tabla de segmentos para un pid en especifico
t_list* create_sg_table(t_memory_structure* structures, int pid);
// Remueve la tabla de segmentos para un pid en especifico
void remove_sg_table(t_memory_structure* structures, int pid);
// adhiere un segmento especifico de un pid en especifico
int add_segment(t_memory_structure* memory_structure, int pid, int size, int s_id);
// Remueve un segmento especifico de un pid en especifico
void delete_segment(t_memory_structure* memory_structure, int pid, int s_id_to_delete);
// Compacta la lista de agujeros libres en caso que estén seguidos sus espacios
void compact_hole_list(t_memory_structure* memory_structure);
// Lee en memoria en base al segmento y el tamaño a leer de este mismo
char* read_memory(int s_id, int offset, int size, t_memory_structure* structures, int pid, char* origen);
// Escribe en memoria en base al segmento y el tamaño a escribir a partir de este mismo
bool write_memory(int s_id, int offset, int size, char* buffer, t_memory_structure* structures, int pid, char* origen);
// Devuelve el segmento en base a su id y del proceso correspondiente
t_segment* get_segment_by_id(int s_id, t_memory_structure* structures, int pid);
// Devuelve el segmento mas cercano al heap
bool more_close_to_heap(void* segment1, void* segment2);
// Compacta la memoria
void compact_memory(t_memory_structure* memory_structure);

int min(int a, int b);
#endif