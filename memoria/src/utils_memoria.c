#include "utils_memoria.h"

t_config_memory config_memory;

void setup_config(char* config_path) {
	config_memory.logger = start_logger("memoria");
	config_memory.config = start_config(config_path);
	config_memory.port = config_get_string_value(config_memory.config, "PUERTO_ESCUCHA");
	config_memory.memory_size = config_get_int_value(config_memory.config, "TAM_MEMORIA");
	config_memory.sg_zero_size = config_get_int_value(config_memory.config, "TAM_SEGMENTO_0");
	config_memory.sg_amount = config_get_int_value(config_memory.config, "CANT_SEGMENTOS");
	config_memory.access_delay = config_get_int_value(config_memory.config, "RETARDO_MEMORIA");
	config_memory.compact_delay = config_get_int_value(config_memory.config, "RETARDO_COMPACTACION");
	config_memory.algorithm = config_get_string_value(config_memory.config, "ALGORITMO_ASIGNACION");
	config_memory.remaining_memory = config_memory.memory_size;
}

t_memory_structure* new_memory_structure(void* memory) {
	// Inicializo las estructuras de memoria
	t_memory_structure* memory_structure = s_malloc(sizeof(t_memory_structure));
	memory_structure->hole_list = list_create();
	memory_structure->table_pid_segments = dictionary_create();
	memory_structure->segment_zero = s_malloc(sizeof(t_segment));
	memory_structure->ram = list_create();
	memory_structure->heap = memory;

	// Creo el segmento 0 y lo agrego al diccionario y a la memoria auxiliar ram
	memory_structure->segment_zero->base = memory;
	memory_structure->segment_zero->offset = config_memory.sg_zero_size;
	memory_structure->segment_zero->s_id = 0;
	dictionary_put(memory_structure->table_pid_segments, (char*)&memory_structure->segment_zero->s_id, memory_structure->segment_zero);
	list_add(memory_structure->ram, memory_structure->segment_zero);
	config_memory.remaining_memory -= config_memory.sg_zero_size;

	// Creo el agujero inicial
	hole* h = s_malloc(sizeof(hole));
	h->base = memory + config_memory.sg_zero_size;
	h->size = config_memory.memory_size - config_memory.sg_zero_size;
	list_add(memory_structure->hole_list, h);

	return memory_structure;
}