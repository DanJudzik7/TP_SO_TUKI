#include "scheduler.h"

void long_term_schedule(t_global_config_kernel* gck) {
	log_info(gck->logger, "Realizando Planificación a Largo Plazo");
	// Remover PCBs terminados por handle_console (o sea, usuario, desconexión, etc)
	for (int i = 0; i < list_size(gck->active_pcbs->elements); i++) {
		t_pcb* pcb = list_get(gck->active_pcbs->elements, i);
		if (pcb->state != EXIT_PROCESS) continue;
		list_remove(gck->active_pcbs->elements, i);
		log_info(gck->logger, "Se removió el proceso terminado %d", pcb->pid);
		if (!socket_send(pcb->pid, package_new(MESSAGE_DONE))) log_error(gck->logger, "Error al informar finalización a la consola %d", pcb->pid);
		socket_close(pcb->pid);
		gck->max_multiprogramming += 1;
		pcb_destroy(pcb);
	}

	// Revisa en base a la multiprogramación si puede agregar más procesos a la cola de Active
	if (queue_size(gck->active_pcbs) < gck->max_multiprogramming && !queue_is_empty(gck->new_pcbs)) {
		gck->max_multiprogramming -= 1;
		t_pcb* pcb = queue_pop(gck->new_pcbs);
		pcb->state = READY;

		// Solicito a Memoria las estructuras necesarias para inicializar
		t_instruction* mem_op = instruction_new(MEM_INIT_PROCESS);
		list_add(mem_op->args, pcb->pid);
		if (!socket_send(gck->socket_memory, serialize_instruction(mem_op))) {
			log_error(gck->logger, "Error al enviar instrucciones a memoria");
		}
		t_package* package = socket_receive(gck->socket_memory);
		if (package->type != pcb->pid) {
			log_error(gck->logger, "Error al inicializar proceso (pid inválido)");
			return;
		}
		pcb->execution_context->segments_table = deserialize_segment_table(package);

		queue_push(gck->active_pcbs, pcb);
		log_info(gck->logger, "El proceso %d ahora está en Ready", pcb->pid);
	} else log_info(gck->logger, "El planificador de largo plazo no tiene ningún PCB posible para ejecutar");
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