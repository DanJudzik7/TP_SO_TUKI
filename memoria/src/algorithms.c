#include "algorithms.h"

segment* first_fit(memory_structure* memory_structure, int size, int pid, int s_id) {
    t_list_iterator* iterator = list_iterator_create(memory_structure->hole_list);
    hole* current_hole;
    while (list_iterator_has_next(iterator)) {
        current_hole = list_iterator_next(iterator);
        if (current_hole->size >= size) {
            // Creamos un nuevo segmento
            segment* new_segment = malloc(sizeof(segment));
            new_segment->base = current_hole->base;
            new_segment->offset = size;
            new_segment->s_id = s_id;
            //TODO: Transformar a una funcion en utility
            char pid_str[10];  
            sprintf(pid_str, "%i", pid);
           
            if (dictionary_has_key(memory_structure->table_pid_segments, pid_str)) {
            t_list* segment_table = dictionary_get(memory_structure->table_pid_segments, pid_str);
            if (segment_table != NULL) {
                list_add_in_index(segment_table, new_segment->s_id, new_segment);
                list_add(memory_structure->ram, new_segment);
            } else {
                    printf("La tabla de segmentos para el proceso con ID %i es nula.\n", pid);
                }
            } else {
                printf("No se encontró la clave %s en el diccionario de segmentos de pid.\n", pid_str);
            }

            // Actualizamos la información del hueco
            current_hole->base += size;
            current_hole->size -= size;

            // Si el hueco se ha vaciado, lo eliminamos de la lista
            if (current_hole->size == 0) {
                list_remove_by_condition(memory_structure->hole_list, (bool (*)(void*)) is_hole_empty);
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

    
segment* best_fit(memory_structure* memory_structure, int size, int pid,int s_id) {
    //TODO: Transformar a una funcion en utility
    char pid_str[10];  
    sprintf(pid_str, "%i", pid);
    hole* best_hole = NULL;
    // Iteramos a través de la lista de huecos.
    for (int i = 0; i < list_size(memory_structure->hole_list); i++) {
        hole* current_hole = list_get(memory_structure->hole_list, i);
        // Si el hueco actual es lo suficientemente grande para alojar el segmento...
        if (current_hole->size >= size) {
            // ...y si no tenemos un "mejor hueco" aún, o si el hueco actual es más pequeño que el "mejor hueco"...
            if (best_hole == NULL || current_hole->size < best_hole->size) {
                // ...entonces el hueco actual se convierte en el "mejor hueco".
                best_hole = current_hole;
            }
        }
    }

    // Si encontramos un "mejor hueco" adecuado...
    if (best_hole != NULL) {
        // ...creamos un nuevo segmento y lo ubicamos en ese hueco.
        segment* new_segment = malloc(sizeof(segment));
        new_segment->base = best_hole->base;
        new_segment->offset = size;
        new_segment->s_id = s_id;  

        // Agregamos el nuevo segmento a la lista de todos los segmentos.
        t_list* segment_table = dictionary_get(memory_structure->table_pid_segments, pid_str);
        list_add(segment_table, new_segment);
        list_add(memory_structure->ram, new_segment);
        

        // Actualizamos la información del hueco.
        best_hole->base += size;
        best_hole->size -= size;

        // Si el hueco se ha vaciado, lo eliminamos de la lista.
        if (best_hole->size == 0) {
            list_remove_by_condition(memory_structure->hole_list, (bool (*)(void*)) is_hole_empty);
        }
        return new_segment;
    }
    // Si no encontramos un hueco lo suficientemente grande, devolvemos NULL.
    else {
        return NULL;
    }
}


segment* worst_fit(memory_structure* memory_structure, int size, int pid,int s_id) {
    //TODO: Transformar a una funcion en utility
    char pid_str[10];  
    sprintf(pid_str, "%i", pid);

    hole* worst_hole = NULL;
    // Iteramos a través de la lista de huecos.
    for (int i = 0; i < list_size(memory_structure->hole_list); i++) {
        hole* current_hole = list_get(memory_structure->hole_list, i);
        // Si el hueco actual es lo suficientemente grande para alojar el segmento...
        if (current_hole->size >= size) {
            // ...y si no tenemos un "peor hueco" aún, o si el hueco actual es más grande que el "peor hueco"...
            if (worst_hole == NULL || current_hole->size > worst_hole->size) {
                // ...entonces el hueco actual se convierte en el "peor hueco".
                worst_hole = current_hole;
            }
        }
    }

    // Si encontramos un "peor hueco" adecuado...
    if (worst_hole != NULL) {
        // ...creamos un nuevo segmento y lo ubicamos en ese hueco.
        segment* new_segment = malloc(sizeof(segment));
        new_segment->base = worst_hole->base;
        new_segment->offset = size;
        new_segment->s_id = s_id;  // Asumiendo que hay un contador global para los IDs de segmentos.

        // Agregamos el nuevo segmento a la lista de todos los segmentos.
        t_list* segment_table = dictionary_get(memory_structure->table_pid_segments, pid_str);
        list_add_in_index(segment_table, new_segment->s_id,new_segment);
        list_add(memory_structure->ram, new_segment);

        // Actualizamos la información del hueco.
        worst_hole->base += size;
        worst_hole->size -= size;

        // Si el hueco se ha vaciado, lo eliminamos de la lista.
        if (worst_hole->size == 0) {
            list_remove_by_condition(memory_structure->hole_list, (bool (*)(void*)) is_hole_empty);
        }

        return new_segment;
    }
    // Si no encontramos un hueco lo suficientemente grande, devolvemos NULL.
    else {
        return NULL;
    }
}