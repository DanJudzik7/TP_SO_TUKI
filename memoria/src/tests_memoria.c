#include "tests_memoria.h"

extern t_config_memory config_memory;

int main() {
	setup_config();
	printf("Iniciando Tests...\n\n");

	//test_segment_creation();
	test_compact();
	// test_rw();

	printf("\n\nTests Completados\n");
	return 0;
}

void express_segment(int pid, int segment, t_memory_structure* memory_structure) {
	create_sg_table(memory_structure, pid);
	printf("Se creó la tabla de segmentos del PID %i", pid);
	add_segment(memory_structure, pid, 128, segment);
}

void test_segment_creation() {
	void* memory = s_malloc(4096);
	t_memory_structure* memory_structure = new_memory_structure(memory);
	config_memory.remaining_memory = config_memory.memory_size;

	express_segment(0, 1, memory_structure);
	express_segment(1, 1, memory_structure);
	express_segment(2, 1, memory_structure);
	express_segment(3, 1, memory_structure);

	// Grafica toda la tabla de segmentos de todos los procesos
	graph_table_pid_segments(memory_structure->table_pid_segments, memory);
	// Función para graficar la RAM
	graph_ram(memory_structure, memory);

	delete_segment(memory_structure, 2, 1);
	delete_segment(memory_structure, 3, 1);
	// Grafica toda la tabla de segmentos de todos los procesos
	graph_table_pid_segments(memory_structure->table_pid_segments, memory);
	// Función para graficar la RAM
	graph_ram(memory_structure, memory);
}

void test_compact() {
	void* memory = s_malloc(4096);
	t_memory_structure* memory_structure = new_memory_structure(memory);
	config_memory.remaining_memory = config_memory.memory_size;

	express_segment(0, 1, memory_structure);
	express_segment(1, 1, memory_structure);
	express_segment(2, 1, memory_structure);
	express_segment(3, 1, memory_structure);
	delete_segment(memory_structure, 1, 1);
	// Función para graficar la RAM
	write_memory(0, 1, 4, "hola", memory_structure, 1);
	graph_ram(memory_structure, memory);

	compact_memory(memory_structure);
	char* buffer = read_memory(0, 1, 4, memory_structure, 1);
	printf("El valor leído es: %s \n", buffer);
	
	t_package* package = serialize_all_segments(memory_structure);
	
	// Función para graficar la RAM
	graph_ram(memory_structure, memory);
	graph_table_pid_segments(memory_structure->table_pid_segments, memory_structure->segment_zero->base);
}

void test_rw() {
	void* memory = s_malloc(4096);
	t_memory_structure* memory_structure = new_memory_structure(memory);
	config_memory.remaining_memory = config_memory.memory_size;

	express_segment(1, 1, memory_structure);
	printf("Iniciando escritura");
	if (write_memory(1, 4, sizeof("hola"), "hola", memory_structure, 1)) {
		printf("Se escribió correctamente en memoria");
		char* buffer = read_memory(1, 4, sizeof("hola"), memory_structure, 1);
		printf("El valor leído es: %s", buffer);
	} else
		printf("No se pudo escribir en memoria");
}

// Graficar la RAM
void graph_ram(t_memory_structure* memory_structure, void* memory_base) {
	t_list* ram = memory_structure->ram;
	t_list* hole_list = memory_structure->hole_list;

	printf("|RAM: \n");

	int ram_size = list_size(ram);
	int hole_list_size = list_size(hole_list);
	int ram_index = 0;
	int hole_index = 0;

	// uintptr_t prev_base = 0;

	// Recorrer tanto los segmentos en RAM como los huecos en hole_list
	while (ram_index < ram_size || hole_index < hole_list_size) {
		uintptr_t ram_base = (ram_index < ram_size) ? ((segment*)list_get(ram, ram_index))->base : UINTPTR_MAX;
		uintptr_t hole_base = (hole_index < hole_list_size) ? ((segment*)list_get(hole_list, hole_index))->base : UINTPTR_MAX;

		if (ram_base <= hole_base) {
			segment* seg = list_get(ram, ram_index);
			printf("| Segmento: %i | base: %u | tamaño: %i |\n", seg->s_id, transform_base_to_decimal(seg->base, memory_base), seg->offset);
			ram_index++;
		} else {
			segment* hole_seg = list_get(hole_list, hole_index);
			printf("|Segmento Hueco | base: %u  | tamaño: %i |\n", transform_base_to_decimal(hole_seg->base, memory_base), hole_seg->offset);
			hole_index++;
		}

		// prev_base = (ram_base <= hole_base) ? ram_base : hole_base;
	}

	printf("\n");
}

// Graficar la tabla table_pid_segments
void graph_table_pid_segments(t_dictionary* table_pid_segments, void* memory_base) {
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

// Graficar una tabla especifica de tipo table_pid_segments
void graph_specific_table_pid_segments(t_list* segment_table, int process_id, void* memory_base) {
	printf("\n|Tabla de segmentos del proceso PID  %i|\n", process_id);
	printf("--------------------------------------\n");
	for (int i = 0; i < list_size(segment_table); i++) {
		segment* seg = list_get(segment_table, i);
		printf("|PID: %i |Segmento: %i | base: %u  | tamaño: %i |\n", process_id, seg->s_id, transform_base_to_decimal(seg->base, memory_base), seg->offset);
	}
	printf("--------------------------------------\n");
}

// Graficar la RAM y la tabla table_pid_segments
void graph_memory(t_memory_structure* memory_struct, void* memory_base) {
	graph_ram(memory_struct, memory_base);
	graph_table_pid_segments(memory_struct->table_pid_segments, memory_base);
}
