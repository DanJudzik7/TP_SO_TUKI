#include "scheduler.h"

void long_term_schedule(t_global_config_kernel* gck) {
	log_info(gck->logger, "Realizando Planificación a Largo Plazo");
	// TODO: LIST_REMOVE_BY_CONDITION no funciona, ACTUALMENTE HARDCODEADO EN KERNEL
	//  Remover PCBs terminados por handle_console (o sea, usuario, desconexión, etc)
	/*
	t_pcb* pcb_removed = list_remove_by_condition(gck->active_pcbs->elements, (void*)pcb_did_exit);
	if (pcb_removed) {
		log_info(gck->logger, "Se removió un proceso terminado");
		gck->max_multiprogramming += 1;
		socket_close(pcb_removed->pid);
		pcb_destroy(pcb_removed);
	}
	*/
	// Revisa en base a la multiprogramación si puede agregar más procesos a la cola de Active
	if (queue_size(gck->active_pcbs) < gck->max_multiprogramming && !queue_is_empty(gck->new_pcbs)) {
		gck->max_multiprogramming -= 1;
		t_pcb* pcb = queue_pop(gck->new_pcbs);
		pcb->state = READY;
		// TODO: Mando el pcb a MEMORIA para que me devuelva las estructuras necesarias para inicializar, a modificar & implementar memoria
		queue_push(gck->active_pcbs, pcb);
		log_info(gck->logger, "El proceso %d ahora está en Ready", pcb->pid);
	} else
		log_info(gck->logger, "El pĺanificador de largo plazo no tiene ningun PCB posible para ejecutar");
}

t_pcb* short_term_scheduler(t_global_config_kernel* gck) {
	if (gck->pcb_priority_helper != NULL) {
		t_pcb* priority_helper = gck->pcb_priority_helper;
		gck->pcb_priority_helper = NULL;
		return priority_helper;
	} else {
		gck->pcb_priority_helper = NULL;
		// Devuelve el próximo PCB a ejecutar en base al algoritmo y los PCBs activos.
		return gck->algorithm_is_hrrn ? pick_with_hrrn(gck->active_pcbs) : pick_with_fifo(gck->active_pcbs);
	}
}

bool pcb_did_exit(t_pcb* pcb) {
	return pcb->state == EXIT_PROCESS;
}