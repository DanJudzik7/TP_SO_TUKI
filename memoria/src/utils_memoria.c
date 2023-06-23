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
void graph_ram(t_list* ram) {
    printf("RAM:\n");
    for (int i = 0; i < list_size(ram); i++) {
        segment* seg = list_get(ram, i);
        printf("Segmento %d: base: %p, offset: %d\n", seg->s_id, seg->base, seg->offset);
    }
    printf("\n");
}

// Función para graficar la tabla table_pid_segments
void graph_table_pid_segments(t_dictionary* table_pid_segments) {
    t_list* keys = dictionary_keys(table_pid_segments);
    for (int i = 0; i < list_size(keys); i++) {
        char* key = string_duplicate(list_get(keys, i));
        int process_id = atoi(key);
        
        t_list* segment_table = dictionary_get(table_pid_segments, key);
        if (segment_table == NULL) {
            free(key); // Liberar la memoria asignada por string_duplicate
            continue; // Pasar a la siguiente clave si no se encuentra la tabla de segmentos
        }
        printf("\nDirección de memoria de segment_table: %p", (void*)segment_table);
        graph_specific_table_pid_segments(segment_table, process_id);
        printf("\n");
        free(key); // Liberar la memoria asignada por string_duplicate
    }
    list_destroy(keys);
}

// Función para graficar una tabla especifica de tipo table_pid_segments
void graph_specific_table_pid_segments(t_list* segment_table, int process_id) {
    printf("\n|    Tabla de segmentos del proceso con ID %d    |\n", process_id);
    printf("|-----------------------------------------------|\n");
    if (segment_table == NULL) {
        printf("La tabla de segmentos es nula.\n");
        return;
    }
    for (int i = 0; i < list_size(segment_table); i++) {
        segment* seg = list_get(segment_table, i);
        printf("|Segmento %d | base: %p | offset %d |\n", seg->s_id, seg->base, seg->offset);
    }
    printf("|-----------------------------------------------|\n\n");
}

// Función para graficar la RAM y la tabla table_pid_segments
void graph_memory(memory_structure* memory_struct) {
    graph_ram(memory_struct->ram);
    graph_table_pid_segments(memory_struct->table_pid_segments);
}