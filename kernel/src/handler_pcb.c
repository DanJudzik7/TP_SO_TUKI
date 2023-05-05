#include "handler_pcb.h"

// TODO MEJORAR CÓDIGO CON LOS LOGGER

void listen_console(global_config_kernel* gck) {
	log_warning(gck->logger, "Entro antes del while");
	while (1) {
		pthread_t thread;

		config_current_process* current = malloc(sizeof(config_current_process));
		current->global_config_kernel = gck;
		int console_socket = socket_accept(gck->connection_kernel);
		if (console_socket == (-1)) {
			log_warning(gck->logger, "Hubo un error aceptando la conexión");
			continue;
		}
		current->current_process = malloc(sizeof(process));
		current->current_process->connection_module_console = console_socket;
		log_info(gck->logger, "Se aceptó una conexión en el socket %d", current->current_process->connection_module_console);
		// Cuando me llega una nueva consola lo mando a un recibidor de nuevo pcb;
		pthread_create(&thread, NULL, (void*)receiver_new_pcb, current);
		pthread_detach(thread);
		// hacer algo con el valor devuelto por long_term_scheduler
	}
}
// TODO: HACER MAS BONITO
void listen_cpu(config_current_process* config_current_process) {
	while (1) {
		config_current_process->current_process->pcb = socket_receive_package(config_current_process->global_config_kernel->connection_module_cpu);
		pthread_t thread;
		pthread_create(&thread, NULL, planificador_kernel, (void*)config_current_process->current_process);
		pthread_detach(thread);
	}
}

t_pcb* create_pcb(process* process, t_log* logger, int default_burst_time) {
	t_log* log = logger;
	t_pcb* pcb = malloc(sizeof(t_pcb));
	pcb->state_pcb = NEW_PROCESS;
	pcb->pid = 2000 + process->connection_module_console;
	pcb->aprox_burst_time = default_burst_time;
	pcb->execution_context = malloc(sizeof(execution_context));
	pcb->execution_context->instructions = queue_create();
	pcb->execution_context->program_counter = 0;
	log_warning(log, "Proceso %d creado en NEW", pcb->pid);
	handle_incoming_instructions(pcb->execution_context->instructions, process->connection_module_console, logger);
	// free(process);
	return pcb;
}

int aprox_burst_time(int value_aprox_time) {
	return value_aprox_time;
}