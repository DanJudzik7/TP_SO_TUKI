#include "algorithms.h"

/*segment* first_fit(structures structures, int size,char* pid,int s_id) {
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
} */