#include "utils_memoria.h"

configuration_memory memory_config;
memory memory_shared;


void initialize(){
    memory_config.logger = start_logger("memoria");
    memory_config.config = start_config("memoria");
    memory_config.port = config_get_string_value(memory_config.config, "PUERTO_ESCUCHA");
    memory_shared.memory_size = config_get_int_value(memory_config.config, "TAM_MEMORIA");
    memory_shared.sg_zero_size = config_get_int_value(memory_config.config, "TAM_SEGMENTO_0");
    memory_shared.sg_amount = config_get_int_value(memory_config.config, "CANT_SEGMENTOS");
    memory_shared.mem_delay = config_get_int_value(memory_config.config, "RETARDO_MEMORIA");
    memory_shared.com_delay = config_get_int_value(memory_config.config, "RETARDO_COMPACTACION");
    memory_shared.algorithm = config_get_string_value(memory_config.config, "ALGORITMO_ASIGNACION");
    memory_shared.remaining_memory = memory_shared.memory_size;
}

// Función para graficar la RAM
void graph_ram(t_list* ram, void* memory_base) {
    printf("|RAM: \n");
    for (int i = 0; i < list_size(ram); i++) {
        segment* seg = list_get(ram, i);
        printf("|Segmento %d | base: %-*lu | offset %d |\n", seg->s_id, sizeof(uintptr_t)/2, transform_base_to_decimal(seg->base, memory_base), seg->offset);
    }
    printf("\n");
}

// Función para graficar la tabla table_pid_segments
void graph_table_pid_segments(t_dictionary* table_pid_segments, void* memory_base) {
    t_list* keys = dictionary_keys(table_pid_segments);
    for (int i = 1; i < list_size(keys); i++) {
        char* key = string_duplicate(list_get(keys, i));
        int process_id = atoi(key);
        printf("\nObteniendo la tabla de segmentos del PID: %s... ", key );
        t_list* segment_table = dictionary_get(table_pid_segments, key);
        if (segment_table == NULL) {
            free(key); // Liberar la memoria asignada por string_duplicate
            continue; // Pasar a la siguiente clave si no se encuentra la tabla de segmentos
        }
        graph_specific_table_pid_segments(segment_table, process_id, memory_base);
        printf("\n");
        free(key); // Liberar la memoria asignada por string_duplicate
    }
    list_destroy(keys);
}

uint32_t transform_base_to_decimal(void* address, void* memory_base) {
    uintptr_t base = (uintptr_t)memory_base;
    uintptr_t transformed_value = (uintptr_t)address;
    return transformed_value - base;
}

// Función para graficar una tabla especifica de tipo table_pid_segments
void graph_specific_table_pid_segments(t_list* segment_table, int process_id, void* memory_base) {
    printf("\n|Tabla de segmentos del proceso PID  %d|\n", process_id);
    printf("--------------------------------------\n");
    for (int i = 0; i < list_size(segment_table); i++) {
        segment* seg = list_get(segment_table, i);
        printf("|Segmento %d | base: %-*lu | offset %d |\n", seg->s_id, sizeof(uintptr_t)/2, transform_base_to_decimal(seg->base, memory_base), seg->offset);
    }
    printf("--------------------------------------\n");
}

// Función para graficar la RAM y la tabla table_pid_segments
void graph_memory(memory_structure* memory_struct, void* memory_base) {
    graph_ram(memory_struct->ram, memory_base);
    graph_table_pid_segments(memory_struct->table_pid_segments, memory_base);
}

