#include "utils_memoria.h"

t_config_memory config_memory;

void setup_config() {
	config_memory.logger = start_logger("memoria");
	config_memory.config = start_config("memoria");
	config_memory.port = config_get_string_value(config_memory.config, "PUERTO_ESCUCHA");
	config_memory.memory_size = config_get_int_value(config_memory.config, "TAM_MEMORIA");
	config_memory.sg_zero_size = config_get_int_value(config_memory.config, "TAM_SEGMENTO_0");
	config_memory.sg_amount = config_get_int_value(config_memory.config, "CANT_SEGMENTOS");
	config_memory.mem_delay = config_get_int_value(config_memory.config, "RETARDO_MEMORIA");
	config_memory.com_delay = config_get_int_value(config_memory.config, "RETARDO_COMPACTACION");
	config_memory.algorithm = config_get_string_value(config_memory.config, "ALGORITMO_ASIGNACION");
	config_memory.remaining_memory = config_memory.memory_size;
}

t_memory_structure* new_memory_structure(void* memory) {
	// Inicializo las estructuras de memoria
	t_memory_structure* memory_structure = s_malloc(sizeof(memory_structure));
	memory_structure->hole_list = list_create();
	memory_structure->table_pid_segments = dictionary_create();
	memory_structure->segment_zero = s_malloc(sizeof(segment));
	// Recordemos que la ram es una t_list unicamente de ayuda, que apunta a las direcciones de memoria [dir_segment_zer, sig dirección, etc ]
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
	hole* hole = malloc(sizeof(hole));
	hole->base = memory + config_memory.sg_zero_size;
	hole->size = config_memory.memory_size - config_memory.sg_zero_size;
	list_add(memory_structure->hole_list, hole);
	// No es necesario cargar el hole en la ram,
	// Cargamos procesos y si eliminamos uno lo mandamos a hole_list pero sigue en la ram hasta que borremos y compactemos
	// list_add(memory_structure->ram,hole);
	return memory_structure;
}