#include "handler_instruction_memoria.h"

extern t_config_memory config_memory;

t_list* create_sg_table(t_memory_structure* memory_structure, int pid) {
	t_list* segment_table = list_create();
	list_add(segment_table, memory_structure->segment_zero);
	char* pid_str = string_itoa(pid);
	dictionary_put(memory_structure->table_pid_segments, pid_str, segment_table);
	return segment_table;
}

void remove_sg_table(t_memory_structure* memory_structure, int pid) {
	char* pid_str = string_itoa(pid);
	t_list* segment_table_delete = dictionary_get(memory_structure->table_pid_segments, pid_str);
	for (int i = 1; i < list_size(segment_table_delete); i++) {
		t_segment* segment = list_get(segment_table_delete, i);
		delete_segment(memory_structure, pid, segment->s_id);
	}

	dictionary_remove(memory_structure->table_pid_segments, pid_str);
}

int add_segment(t_memory_structure* memory_structure, int process_id, int size, int s_id) {
	t_segment* segment;

	if (config_memory.remaining_memory > size) {
		// Recorro la ram para asignar el segmento según el algoritmo
		if (strcmp(config_memory.algorithm, "BEST") == 0)
			segment = best_fit(memory_structure, size, process_id, s_id);
		else if (strcmp(config_memory.algorithm, "FIRST") == 0)
			segment = first_fit(memory_structure, size, process_id, s_id);
		else if (strcmp(config_memory.algorithm, "WORST") == 0)
			segment = worst_fit(memory_structure, size, process_id, s_id);
		else {
			log_error(config_memory.logger, "No se reconoce el algoritmo de planificación");
			abort();
		}
		// Si el segmento es nulo pero tengo espacio, debo compactar
		if (segment != NULL) {
			int dir_base = transform_base_to_decimal(segment->base, memory_structure->segment_zero->base);
			config_memory.remaining_memory -= size;
			log_info(config_memory.logger, "| PID: %i | Crear Segmento: %i | Base: %i | Tamaño: %i |", process_id, segment->s_id, dir_base, size);
			return dir_base;  // Segmento creado
		}
		// Si el segmento es nulo pero tengo espacio, debo compactar
		else return -1;  // Compactación
	} else {
		log_error(config_memory.logger, "No hay espacio suficiente para crear el segmento");
		// Manejo el error devolviendo a kernel no hay espacio suficiente
		return -2;  // No hay espacio
	}
}
void delete_segment(t_memory_structure* memory_structure, int pid, int s_id_to_delete) {
	char* pid_str = string_itoa(pid);
	t_segment* segment_to_delete = NULL;

	// Obtengo la lista de segmentos del proceso a eliminar
	t_list* segment_table = dictionary_get(memory_structure->table_pid_segments, pid_str);
	// Obtengo el segmento que coincida con el que quiero eliminar en la tabla de su pid
	for (int i = 1; i < list_size(segment_table); i++) {
		t_segment* segment_pid = list_get(segment_table, i);
		if (segment_pid->s_id == s_id_to_delete) {
			// Asigno para tener el segmento más a mano
			segment_to_delete = segment_pid;
			segment_to_delete->offset = segment_pid->offset;
			config_memory.remaining_memory += segment_pid->offset;
			list_add(memory_structure->hole_list, segment_pid);
			list_remove(segment_table, i);
			break;
		}
	}

	// Recorro la RAM para eliminarla
	for (int i = 1; i < list_size(memory_structure->ram); i++) {
		t_segment* segment_ram = list_get(memory_structure->ram, i);
		// Si tiene la misma base que el segmento a eliminar
		if (segment_ram->base == segment_to_delete->base) {
			// Elimino justo esa posición
			list_remove(memory_structure->ram, i);
			break;
		}
	}
	int dir_base = transform_base_to_decimal(segment_to_delete->base, memory_structure->segment_zero->base);
	log_info(config_memory.logger, "| PID: %i | Eliminar Segmento: %i | Base: %i  | Tamaño: %i |", pid, s_id_to_delete, dir_base, segment_to_delete->offset);

	// Caso de que tenga huecos libres aledaños, los deberá consolidar actualizando sus estructuras administrativas.
	compact_hole_list(memory_structure);
}

void compact_hole_list(t_memory_structure* memory_structure) {
	if (list_size(memory_structure->hole_list) != 1) {
		for (int i = 0; i < list_size(memory_structure->hole_list); i++) {
			// Mientras segment no sea el ultimo
			if (i < list_size(memory_structure->hole_list) - 1) {
				t_segment* segment_current = list_get(memory_structure->hole_list, i);
				// agarro su siguiente segmento para comparar
				t_segment* segment_next = list_get(memory_structure->hole_list, i + 1);
				// Si el sig segmento comienza dps del anterior los compacto y elimino el siguiente
				if (segment_current->base + segment_current->offset == segment_next->base) {
					segment_current->offset += segment_next->offset;
					list_remove(memory_structure->hole_list, i + 1);
					// Como compacte la memoria eliminando uno, el size se acorta por lo tanto debo avanzar 1 mas
					i++;
				}
			}
		}
	}
}

void swap(t_segment* a, t_segment* b) {
	t_segment temp;
	memcpy(&temp, a, sizeof(t_segment));
	memcpy(a, b, sizeof(t_segment));
	memcpy(b, &temp, sizeof(t_segment));
}

bool more_close_to_heap(void* segment1, void* segment2) {
	t_segment* seg1 = (t_segment*)segment1;
	t_segment* seg2 = (t_segment*)segment2;
	return seg1->base < seg2->base;
}

void compact_memory(t_memory_structure* memory_structure) {
	int size_of_hole = list_size(memory_structure->hole_list);
	if (size_of_hole > 1) {
		// Ordeno la lista de hole para tener siempre a mano el mas cercano al heap
		list_sort(memory_structure->hole_list, more_close_to_heap);
		// Recorro la memoria ram auxiliar donde están mis procesos actuales
		for (int i = 1; i < list_size(memory_structure->ram); i++) {
			// Agarro la posición de memoria en la ram
			t_segment* ram_segment = list_get(memory_structure->ram, i);
			// agarro el primer hole que siempre va a ser el mas cercano al heap
			t_segment* hole_segment = list_get(memory_structure->hole_list, 0);
			// Si el segmento en ram esta mas abajo que el hueco (osea tengo un hueco arriba en ram), hago el swap
			if (ram_segment->base > hole_segment->base) {
				t_segment temp;
				memcpy(&temp, hole_segment, sizeof(t_segment));
				memcpy(hole_segment, ram_segment, sizeof(t_segment));
				memcpy(ram_segment, &temp, sizeof(t_segment));

				i--;  // Disminuyo el índice para mantenerlo en la posición correcta en el siguiente ciclo
			}
			list_sort(memory_structure->hole_list, more_close_to_heap);
		}
	}
	compact_hole_list(memory_structure);
}

char* read_memory(int s_id, int offset, int size, t_memory_structure* structures, int pid) {
	char* buffer = s_malloc(size + 1);
	t_segment* segment = get_segment_by_id(s_id, structures, pid);
	if (segment != NULL) {
		if (segment->base + offset + size > segment->base + segment->offset) {
			log_error(config_memory.logger, "Segmentation fault, no se puede leer mas allá del segment");
			return NULL;
		} else {
			memcpy(buffer, segment->base + offset, sizeof(size));
			buffer[size] = '\0';
			return buffer;
		}
	} else {
		log_error(config_memory.logger, "No se encontró el segmento solicitado");
		return NULL;
	}
}

bool write_memory(int s_id, int offset, int size, char* buffer, t_memory_structure* structures, int pid) {
	t_segment* segment = get_segment_by_id(s_id, structures, pid);
	if (segment != NULL) {
		if (segment->base + offset + size > segment->base + segment->offset) {
			log_error(config_memory.logger, "Segmentation fault, no se puede escribir mas allá del segment");
			return false;
		} else {
			memcpy(segment->base + offset, buffer, strlen(buffer) + 1);
			return true;
		}
	} else {
		log_error(config_memory.logger, "No se encontró el segmento solicitado");
		return false;
	}
}

t_segment* get_segment_by_id(int s_id, t_memory_structure* structures, int pid) {
	char* pid_str = string_itoa(pid);
	t_list* segment_table = dictionary_get(structures->table_pid_segments, pid_str);

	t_segment* segment;
	for (int i = 0; i < list_size(segment_table); i++) {
		segment = list_get(segment_table, i);
		if (segment->s_id == s_id) {
			return segment;
		}
	}
	return NULL;
}

uint32_t transform_base_to_decimal(void* address, void* memory_base) {
	uintptr_t base = (uintptr_t)memory_base;
	uintptr_t transformed_value = (uintptr_t)address;
	return transformed_value - base;
}