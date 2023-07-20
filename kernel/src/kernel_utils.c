#include "kernel_utils.h"

t_global_config_kernel* new_global_config_kernel(t_config* config) {
	t_global_config_kernel* gck = s_malloc(sizeof(t_global_config_kernel));
	gck->logger = start_logger("kernel");

	// Obtengo del config el algoritmo a usar
	char* algorithm = config_get_string_value(config, "ALGORITMO_PLANIFICACION");
	bool algorithm_is_hrrn = strcmp(algorithm, "HRRN") == 0;
	if (!algorithm_is_hrrn && strcmp(algorithm, "FIFO") != 0) {
		log_error(gck->logger, "El algoritmo de planificación no es válido");
		exit(EXIT_FAILURE);
	}
	log_warning(gck->logger, "El algoritmo de planificación es: %s", algorithm);

	gck->new_pcbs = queue_create();		// Cola local de PCBs en NEW
	gck->active_pcbs = queue_create();	// Cola global de PCBs en READY, EXEC y BLOCK
	gck->server_socket = -1;

	char* max_multiprogramming = config_get_string_value(config, "GRADO_MAX_MULTIPROGRAMACION");
	gck->max_multiprogramming = atoi(max_multiprogramming);
	free(max_multiprogramming);
	char* default_burst_time = config_get_string_value(config, "ESTIMACION_INICIAL");
	gck->default_burst_time = atoi(default_burst_time);
	free(default_burst_time);
	gck->algorithm_is_hrrn = algorithm_is_hrrn;
	gck->prioritized_pcb = NULL;
	gck->resources = dictionary_create();
	return gck;
}

void handle_pcb_io(t_helper_pcb_io* hpi) {
	sleep(hpi->time);
	hpi->pcb->state = READY;
	log_warning(hpi->logger, "El proceso %d se desbloqueó", hpi->pcb->pid);
	free(hpi);
}

t_resource* resource_get(t_pcb* pcb, t_global_config_kernel* gck, char* name) {
	if (!dictionary_has_key(gck->resources, name)) {
		pcb->state = EXIT_PROCESS;
		log_error(gck->logger, "El recurso %s no existe", name);
		return NULL;
	}
	log_info(gck->logger, "Se solicitó el recurso %s", name);
	return dictionary_get(gck->resources, name);
}

void resource_signal(t_resource* resource, t_log* logger) {
	if (!queue_is_empty(resource->enqueued_processes)) {
		resource->assigned_to = queue_pop(resource->enqueued_processes);
		resource->assigned_to->state = READY;
		log_info(logger, "Se desbloqueó el proceso %d", resource->assigned_to->pid);
	} else {
		resource->available_instances++;
		resource->assigned_to = NULL;
		log_info(logger, "Las instancias del recurso aumentaron a %i", resource->available_instances);
	}
}

t_pcb* pcb_new(int pid, int burst_time) {
	t_pcb* pcb = s_malloc(sizeof(t_pcb));
	pcb->state = NEW;
	pcb->pid = pid;
	pcb->aprox_burst_time = burst_time;
	pcb->last_burst_time = 0;
	pcb->last_ready_time = time(NULL);
	pcb->local_files = dictionary_create();
	pcb->execution_context = execution_context_new(pcb->pid);
	return pcb;
}

void pcb_destroy(t_pcb* pcb) {
	execution_context_destroy(pcb->execution_context);
	dictionary_destroy(pcb->local_files);
	pcb->local_files = NULL;
	free(pcb);
	pcb = NULL;
}