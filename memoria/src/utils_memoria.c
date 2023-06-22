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
    memory_shared.algorithm = config_get_string_value(memory_config.config, "ALGORITMO_PLANIFICACION");
}

void create_structure(structures structures){
    t_list* segment_table = list_create();
    list_add(segment_table,structures.segment_zero);
    dictionary_put(structures.all_segments , "id1", segment_table); //TODO: hacer que el id1 se modular 
}

void remove_structure(structures structures){
    dictionary_remove(structures.all_segments,"id1"); //TODO: hacer que el id1 se modular 
}