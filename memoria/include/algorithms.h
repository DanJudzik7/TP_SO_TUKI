#ifndef ALGORITHMS_H
#define ALGORITHMS_H

#include "utils_memoria.h"

/*segment* first_fit(structures structures, int size, char* pid, int s_id);
bool is_hole_empty(hole* h);
*/
segment* first_fit(memory_structure* structures, int size, int pid, int s_id); 
bool is_hole_empty(hole* h);
segment* best_fit(memory_structure* structures, int size, int pid, int s_id);
segment* worst_fit(memory_structure* structures, int size, int pid, int s_id);

#endif