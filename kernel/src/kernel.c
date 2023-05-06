#include "kernel.h"

int main(int argc, char** argv) {
	t_log* logger = start_logger("kernel");
	t_config* config = start_config("kernel");
	log_warning(logger, "Iniciando el kernel");
	char* port = config_get_string_value(config, "PUERTO_ESCUCHA");	 // Obtenemos el puerto con el que escucharemos conexiones
	int socket_kernel = socket_initialize_server(port);	// Inicializo el socket en el puerto cargado por la config
	if (socket_kernel == -1) {
		log_error(logger, "No se pudo inicializar el socket de servidor");
		exit(EXIT_FAILURE);
	}
	log_warning(logger, "Socket de servidor inicializado en puerto %s", port);

	// Connect_module conecta al modulo que le pasa como tercer parámetros
	// int socket_cpu =  connect_module(config, logger, "CPU");
	// int conn_memoria = connect_module(config, logger, "MEMORIA");
	// int conn_filesystem = connect_module(config, logger, "FILESYSTEM");

	// Obtengo del config el algoritmo a usar
	char* algorithm = config_get_string_value(config, "ALGORITMO_PLANIFICACION");
	bool algorithm_is_hrrn = strcmp(algorithm, "HRRN") == 0;
	if (!algorithm_is_hrrn && strcmp(algorithm, "FIFO") != 0) {
		log_error(logger, "El algoritmo de planificación no es válido");
		exit(EXIT_FAILURE);
	}
	log_warning(logger, "El algoritmo de planificación es: %s", algorithm);

	t_global_config_kernel* gck = malloc(sizeof(t_global_config_kernel));
	gck->logger = logger;
	gck->new_pcbs = queue_create(); // Cola local de PCBs en NEW
	gck->active_pcbs = queue_create(); // Cola global de PCBs en READY, EXEC y BLOCK
	gck->connection_kernel = socket_kernel;
	gck->max_multiprogramming = *config_get_string_value(config, "GRADO_MAX_MULTIPROGRAMACION") - '0';
	gck->default_burst_time = *config_get_string_value(config, "ESTIMACION_INICIAL") - '0';
	gck->algorithm_is_hrrn = algorithm_is_hrrn;

	// Manejo de consolas
	pthread_t thread_consola;
	pthread_create(&thread_consola, NULL, (void*) listen_consoles, gck);

	// Manejo de CPU y Short Term Scheduler. Antes era listener_cpu.
	while (1) {
		// Organizo según el tipo de planificador
		t_pcb* pcb = short_term_scheduler(gck);
		if (pcb == NULL || pcb == 0) continue;
		pcb->state = EXEC;
		log_info(logger, "Enviando a CPU el Execution Context del proceso %d", pcb->pid);
		// TO DO: Mandar a CPU y esperar
		// Recibe el nuevo execution context
		// pcb->execution_context = socket_receive_package(socket_cpu);
		queue_clean(pcb->execution_context->instructions); // WORKAROUND TEMPORAL. Ya que todavía la conexión a CPU no está, borro todas las instrucciones para simular su ejecución.
		// Revisa si está bloqueado
		pcb->state = pcb->execution_context->updated_state;
		// Nota: EXIT es solo finalización implícita del proceso (según la consigna, usuario y error). El completado de instrucciones debe devolver READY.
		if (queue_is_empty(pcb->execution_context->instructions)) pcb->state = BLOCK;
		if (pcb->state == EXIT_PROCESS) long_term_schedule(gck);
		else queue_push(gck->active_pcbs, pcb);
	}
}