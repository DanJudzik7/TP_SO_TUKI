#ifndef ALGORITHMS_H
#define ALGORITHMS_H

#include "utils_memoria.h"

/*segment* first_fit(structures structures, int size, char* pid, int s_id);
bool is_hole_empty(hole* h);
*/
t_segment* first_fit(t_memory_structure* structures, int size, int pid, int s_id);
bool is_hole_empty(hole* h);
t_segment* best_fit(t_memory_structure* structures, int size, int pid, int s_id);
t_segment* worst_fit(t_memory_structure* structures, int size, int pid, int s_id);

#endif