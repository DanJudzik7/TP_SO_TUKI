#include "utils_memoria.h"

t_config_memory config_memory;

void setup_config() {
	config_memory.logger = start_logger("memoria");
	config_memory.config = start_config("memoria");
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
	// Recordemos que la ram es una t_list unicamente de ayuda, que apunta a las direcciones de memoria [dir_segment_zer, sig dirección, etc ]
	memory_structure->ram = list_create();
	memory_structure->heap = memory;

	// Creo el segmento 0 y lo agrego al diccionario y a la memoria auxiliar ram
	memory_structure->segment_zero->base = memory;
	memory_structure->segment_zero->offset = (int)config_memory.sg_zero_size;
	memory_structure->segment_zero->s_id = 0;
	dictionary_put(memory_structure->table_pid_segments, (char*)&memory_structure->segment_zero->s_id, memory_structure->segment_zero);
	list_add(memory_structure->ram, memory_structure->segment_zero);
	config_memory.remaining_memory -= config_memory.sg_zero_size;

	// Creo el agujero inicial
	hole* h = s_malloc(sizeof(hole));
	h->base = memory + config_memory.sg_zero_size;
	h->size = config_memory.memory_size - config_memory.sg_zero_size;
	list_add(memory_structure->hole_list, h);
	// No es necesario cargar el hole en la ram,
	// Cargamos procesos y si eliminamos uno lo mandamos a hole_list pero sigue en la ram hasta que borremos y compactemos
	// list_add(memory_structure->ram,hole);
	return memory_structure;
}

// Graficar una tabla especifica de tipo table_pid_segments
void graph_specific_table_pid_segments(t_list* segment_table, int process_id, t_memory_structure* memory_base) {
	printf("\n|Tabla de segmentos del proceso PID  %i|\n", process_id);
	printf("--------------------------------------\n");
	for (int i = 0; i < list_size(segment_table); i++) {
		t_segment* seg = list_get(segment_table, i);
		printf("| PID: %i |Segmento: %i | base: %u  | tamaño: %i |\n", process_id, seg->s_id, transform_base_to_decimal(seg->base, memory_base->segment_zero->base), seg->offset);
	}
	printf("--------------------------------------\n");
}


// Graficar la tabla table_pid_segments
void graph_table_pid_segments(t_dictionary* table_pid_segments, t_memory_structure* memory_base) {
	t_list* keys = dictionary_keys(table_pid_segments);
	for (int i = 1; i < list_size(keys); i++) {
		char* key = string_duplicate(list_get(keys, i));
		int process_id = atoi(key);
		printf("\nObteniendo la tabla de segmentos del PID: %s... ", key);
		t_list* segment_table = dictionary_get(table_pid_segments, key);
		if (segment_table == NULL) {
			free(key);	// Liberar la memoria asignada por string_duplicate
			continue;	// Pasar a la siguiente clave si no se encuentra la tabla de segmentos
		}
		graph_specific_table_pid_segments(segment_table, process_id, memory_base);
		printf("\n");
		free(key);	// Liberar la memoria asignada por string_duplicate
	}
	list_destroy(keys);
}