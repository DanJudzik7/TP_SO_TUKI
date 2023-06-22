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
    memory_shared.remaining_memory = memory_shared.memory_size;
}

t_list* create_sg_table(structures structures,char* process_id){
    t_list* segment_table = list_create();
    list_add(segment_table,structures.segment_zero);
    dictionary_put(structures.all_segments , process_id, segment_table); 
    return segment_table;
}

void remove_sg_table(structures structures,char* process_id){
    dictionary_remove(structures.all_segments,process_id); 
}

void add_segment(structures structures,char* process_id, int size,int s_id){
    if (memory_shared.remaining_memory > size)
    {
       //recorro segun algoritmo
       if (strcmp(memory_shared.algorithm,"BEST") == 0){
           // best_fit(structures,size);
       } else if (strcmp(memory_shared.algorithm,"FIRST") == 0){
            first_fit(structures,size,process_id,s_id);
       } else if (strcmp(memory_shared.algorithm,"WORST") == 0){
            //worst_fit(structures,size);
       } else{
           log_error(memory_config.logger,"No se reconoce el algoritmo de planificacion");
           exit(1);
       }
    } else{
        log_error(memory_config.logger,"No hay espacio suficiente para crear el segmento");
        // Manejo el error devolviendo a kernel no hay espacio suficiente
    }
    

}

segment* first_fit(structures structures, int size,char* pid,int s_id) {
    t_list_iterator* iterator = list_iterator_create(structures.hole_list);
    hole* current_hole;
    while (list_iterator_has_next(iterator)) {
        current_hole = list_iterator_next(iterator);
        if (current_hole->size >= size) {
            // Creamos un nuevo segmento
            segment* new_segment = malloc(sizeof(segment));
            new_segment->base = current_hole->base;
            new_segment->offset = size;
            // Asumiendo que s_id es un contador global para los ID de segmentos.
            new_segment->s_id = s_id;

            // Agregamos el nuevo segmento a la lista de todos los segmentos
            t_list* segment_table = dictionary_get(structures.all_segments, pid);
            list_add_in_index(segment_table, new_segment->s_id,new_segment);
            list_add(structures.ram, new_segment);

            // Actualizamos la información del hueco
            current_hole->base += size;
            current_hole->size -= size;

            // Si el hueco se ha vaciado, lo eliminamos de la lista
            if (current_hole->size == 0) {
                list_remove_by_condition(structures.hole_list, (bool (*)(void*)) is_hole_empty);
            }

            list_iterator_destroy(iterator);
            return new_segment;
        }
    }
    list_iterator_destroy(iterator);
    return NULL; // Si no se encuentra un hueco suficientemente grande, se devuelve NULL
}

bool is_hole_empty(hole* h) {
    return h->size == 0;
}