#include "handler_pcb.h"

void listen_console(t_console_init* ci) {
    while (1) {
        pthread_t thread;
        int* console_socket = malloc(sizeof(int));
        *console_socket = socket_accept(ci->server_fd);
        if (*console_socket == (-1)) {
            log_warning(ci->logger, "Hubo un error aceptando la conexión");
            continue;
        }
        ci->console = *console_socket;
        log_debug(ci->logger, "Se aceptó una conexión en el socket %d", *console_socket);
        op_code_reception result;
        pthread_create(&thread, NULL, long_term_scheduler, (void*) ci);
        pthread_join(thread, (void**) &result);
		socket_send_message(result, console_socket);
        free(console_socket);
         // hacer algo con el valor devuelto por long_term_scheduler
    }
}
void listen_cpu(t_console_init* ci) {
    while (1) {
		ci -> pcb = socket_receive_message(ci->conection_module);
        pthread_t thread;
        pthread_create(&thread, NULL, planificador_kernel, (void*) ci);
		pthread_detach(thread);
    }
}

t_pcb* create_pcb(t_console_init* ci) {
	t_log* logger = ci->logger;
	t_pcb* pcb = malloc(sizeof(t_pcb));
	pcb->state_pcb = NEW;
	pcb->pid = 2000 + ci->console;
	pcb->aprox_burst_time = ci->default_burst_time;
	pcb->execution_context = malloc(sizeof(execution_context));
	pcb->execution_context->instructions = queue_create();
	pcb->execution_context->program_counter = 0;
	log_info(logger, "Proceso %d creado en NEW", pcb->pid);
	handle_incoming_instructions(pcb->execution_context->instructions, ci->console, logger);
	free(ci);
	return pcb;
}

int aprox_burst_time(int value_aprox_time) {
	return value_aprox_time;
}