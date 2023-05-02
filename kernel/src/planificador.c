#include "planificador.h"

t_pcb* planificador_kernel(t_log* logger, t_pcb* pcb_received, char* algorithm, t_queue* queue_global_pcb) {
	log_info(logger, "llego");
	t_pcb* pcb_send = malloc(sizeof(t_pcb));
	if (strcmp(algorithm, "FIFO") == 0) {  // Organizo según el tipo de planificador
		pcb_send = fifo(queue_global_pcb, pcb_received);
	} else if (strcmp(algorithm, "HRRN") == 0) {
		pcb_send = hrrn(logger, queue_global_pcb, pcb_received);
	}
	return pcb_send;
}