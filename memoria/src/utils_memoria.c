#include "utils_memoria.h"

configuration_memory config_memory;
memory memory_shared;

void initialize(){
    config_memory.logger = start_logger("memoria");
    config_memory.config = start_config("memoria");
    config_memory.port = config_get_string_value(config_memory.config, "PUERTO_ESCUCHA");
    memory_shared.memory_size = config_get_string_value(config_memory.config, "TAM_MEMORIA");
    memory_shared.sg_zero_size = config_get_string_value(config_memory.config, "TAM_SEGMENTO_0");
    memory_shared.sg_amount = config_get_string_value(config_memory.config, "CANT_SEGMENTOS");
    memory_shared.mem_delay = config_get_string_value(config_memory.config, "RETARDO_MEMORIA");
    memory_shared.com_delay = config_get_string_value(config_memory.config, "RETARDO_COMPACTACION");
    memory_shared.algorithm = config_get_string_value(config_memory.config, "ALGORITMO_PLANIFICACION");

}