#include "algorithms_kernel.h"

t_pcb* pick_with_fifo(t_queue* active_pcbs) {
	// Siempre devuelve NULL si está vacía
	if (queue_is_empty(active_pcbs)) return NULL;
	// Obtiene el primer elemento READY, lo borra de la queue y lo devuelve
	// Trabaja a la queue como una list para esto
	return list_remove_by_condition(active_pcbs->elements, (void*)pcb_is_ready);

}

bool pcb_is_ready(t_pcb* pcb) {
	return pcb->state == READY;
}

// Esto se tiene que adaptar para no devolver nada con BLOCK
t_pcb* pick_with_hrrn(t_queue* active_pcbs) {
	if (queue_is_empty(active_pcbs)) return NULL;
	if (queue_size(active_pcbs) == 1) {
		t_pcb* pcb = queue_peek(active_pcbs);
		return pcb->state == READY ? queue_pop(active_pcbs) : NULL;
	}
	
	// Se obtiene la hora actual del sistema
	time_t current_time = time(NULL);

	// Se inicializa la cola temporal
	t_queue* queue_temporal = queue_create();
	

	// Se inicializan los valores del response ratio
	float response_ratio = 0.0;
	float highest_response_ratio = -1.0;

	// Se inicializa el PCB siguiente como nulo
	t_pcb* next_pcb = NULL;
	// Se inicializa el PCB actual como nulo
	t_pcb* p = NULL;

	// Se itera sobre la cola global de PCBs
	while (!queue_is_empty(active_pcbs)) {
		// Se obtiene el primer PCB de la cola
		p = queue_peek(active_pcbs);

		// Se calcula el response ratio del proceso actual
		response_ratio = (float)(current_time - p->last_ready_time + p->aprox_burst_time) / (float)p->aprox_burst_time;

		// Si el response ratio del proceso actual es mayor que el response ratio más alto encontrado
		if (response_ratio > highest_response_ratio) {
			// Se actualiza el response ratio más alto encontrado
			highest_response_ratio = response_ratio;

			// Si el PCB siguiente no es nulo, se agrega a la cola temporal
			if (next_pcb != NULL && pcb_is_ready(next_pcb)) {  // Esta línea la modifiqué para que se asegure de que el PCB esté listo
				queue_push(queue_temporal, next_pcb);
			}

			// El siguiente PCB es el proceso actual
			next_pcb = p;
		} else {
			// Si el response ratio del proceso actual no es mayor que el response ratio más alto encontrado, se agrega a la cola temporal
			queue_push(queue_temporal, p);
		}

		// Se elimina el proceso actual de la cola global
		queue_pop(active_pcbs);
	}
	// Se libera la memoria del proceso actual
	free(p);
	// Se limpia la cola global de PCBs
	queue_clean(active_pcbs);

	// La cola global de PCBs ahora es la cola temporal
	active_pcbs = queue_temporal;

	// Se libera la memoria de la cola temporal
	free(queue_temporal);

	// Se actualiza el last_ready_time del proceso seleccionado
	next_pcb->last_ready_time = current_time;

	// Se devuelve el proceso con el mayor response ratio
	if (next_pcb != NULL && pcb_is_ready(next_pcb)) {
		// Actualiza el last_ready_time del proceso seleccionado
		next_pcb->last_ready_time = current_time;
		// Devuelve el proceso con el mayor response ratio
		return next_pcb;
	} else {
		// Si next_pcb no es válido o no está listo, asegúrate de liberar la memoria asignada
		free(next_pcb);
		return NULL;
	}
}