#include "scheduler.h"

void long_term_schedule(t_global_config_kernel* gck) {
	pthread_mutex_lock(&(gck->long_term_scheduler_execution));
	log_info(gck->logger, "Realizando Planificación a Largo Plazo");
	// Remover PCBs terminados por handle_new_process (o sea, usuario, desconexión, etc)
	for (int i = 0; i < list_size(gck->active_pcbs->elements); i++) {
		t_pcb* pcb = list_get(gck->active_pcbs->elements, i);
		if (pcb->state != EXIT_PROCESS) continue;
		list_remove(gck->active_pcbs->elements, i);
		log_info(gck->logger, "Se removió el proceso terminado %d", pcb->pid);

		// Desconecto de la consola
		if (!socket_send(pcb->pid, package_new(MESSAGE_DONE)))
			log_error(gck->logger, "Error al informar finalización a la consola %d", pcb->pid);
		socket_close(pcb->pid);

		// Libero todos los recursos
		t_list* resources_names = dictionary_keys(gck->resources);
		for (int i = 0; i < list_size(resources_names); i++) {
			char* resource_name = list_get(resources_names, i);
			t_resource* resource = dictionary_get(gck->resources, resource_name);
			if (resource->assigned_to != NULL && resource->assigned_to == pcb) resource_signal(resource, resource_name, gck->logger);
			t_list* resource_list = resource->enqueued_processes->elements;
			for (int j = 0; j < list_size(resource_list); j++) {
				if (list_get(resource_list, j) == pcb) list_remove(resource_list, j);
			}
		}

		// Destruyo las estructuras
		t_instruction* mem_op = instruction_new(MEM_END_PROCCESS);
		list_add(mem_op->args, string_itoa(pcb->pid));
		if (!socket_send(gck->socket_memory, serialize_instruction(mem_op))) {
			log_error(gck->logger, "Error al enviar instrucciones a memoria");
		}
		t_package* package = socket_receive(gck->socket_memory);
		if (package == NULL || package->type != OK_INSTRUCTION) {
			log_error(gck->logger, "Error al eliminar estructuras del proceso");
			abort();
		}
		pcb->execution_context->segments_table = deserialize_segment_table(package);

		pcb_destroy(pcb);
	}

	// Revisa en base a la multiprogramación si puede agregar más procesos a la cola de Active
	if (queue_size(gck->active_pcbs) < gck->max_multiprogramming && !queue_is_empty(gck->new_pcbs)) {
		t_pcb* pcb = queue_pop(gck->new_pcbs);
		log_warning(gck->logger, "PID: %d - Estado Anterior: NEW - Estado Actual: READY", pcb->pid);
		pcb->state = READY;

		// Solicito a Memoria las estructuras necesarias para inicializar
		t_instruction* mem_op = instruction_new(MEM_INIT_PROCESS);
		list_add(mem_op->args, string_itoa(pcb->pid));
		if (!socket_send(gck->socket_memory, serialize_instruction(mem_op))) {
			log_error(gck->logger, "Error al enviar instrucciones a memoria");
		}
		t_package* package = socket_receive(gck->socket_memory);
		if (package == NULL || package->type != SEGMENTS_TABLE) {
			log_error(gck->logger, "Error al inicializar estructuras del proceso");
			abort();
		}
		pcb->execution_context->segments_table = deserialize_segment_table(package);

		queue_push(gck->active_pcbs, pcb);
		log_info(gck->logger, "El proceso %d ahora está en Ready", pcb->pid);
	} else
		log_info(gck->logger, "El planificador de largo plazo no tiene ningún PCB posible para ejecutar");
	show_queue_ready(gck);
	pthread_mutex_unlock(&(gck->long_term_scheduler_execution));
}

t_pcb* short_term_scheduler(t_global_config_kernel* gck) {
	sleep(1);
	if (gck->prioritized_pcb != NULL) {
		t_pcb* prioritized = gck->prioritized_pcb;
		gck->prioritized_pcb = NULL;
		list_remove_element(gck->active_pcbs->elements, prioritized);
		return prioritized;
	} else {
		// Devuelve el próximo PCB a ejecutar en base al algoritmo y los PCBs activos.
		return gck->algorithm_is_hrrn ? pick_with_hrrn(gck->active_pcbs) : pick_with_fifo(gck->active_pcbs);
	}
}

void show_queue_ready(t_global_config_kernel* gck){

	char* pids_list = string_new();
	for (int i = 0; i < queue_size(gck->active_pcbs); i++) {
		t_pcb* pcb = list_get(gck->active_pcbs->elements, i);
		if (pcb->state != READY) continue;
		string_append_with_format(&pids_list, "%d", pcb->pid);
		if (i < queue_size(gck->active_pcbs) - 1) string_append(&pids_list, ", ");
	}
	
	log_warning(gck->logger, "Cola Ready %s: [%s]", gck->algorithm_is_hrrn ? "HRRN" : "FIFO", pids_list);
}
