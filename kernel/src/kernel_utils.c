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
	gck->connection_kernel = NULL;
	// TODO: ESTO es realmente int no un string_value
	gck->max_multiprogramming = *config_get_string_value(config, "GRADO_MAX_MULTIPROGRAMACION") - '0';
	gck->default_burst_time = *config_get_string_value(config, "ESTIMACION_INICIAL") - '0';
	gck->algorithm_is_hrrn = algorithm_is_hrrn;
	gck->pcb_priority_helper = NULL;
	gck->resources = dictionary_create();
	return gck;
}

void exit_process(t_pcb* pcb, t_global_config_kernel* gck) {
	log_warning(gck->logger, "----------------------El PCB %d tiene estado exit----------------------", pcb->pid);
	pcb->state = EXIT_PROCESS;
	// TODO: HARDCODEADO hasta que se pueda mover a long_term_shedule
	long_term_schedule(gck);
	sem_post(&gck->flag_with_pcb);
}