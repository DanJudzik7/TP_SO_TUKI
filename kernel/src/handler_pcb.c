#include "handler_pcb.h"

//TODO MEJORAR CODIGO CON LOS LOGGER
void listen_console(config_current_process* config_current_process) {

    while (1) {
        pthread_t thread;
        int* console_socket = malloc(sizeof(int));
        *console_socket = socket_accept(config_current_process -> global_config_kernel -> conection_kernel);
        if (*console_socket == (-1)) {
            log_warning(config_current_process -> global_config_kernel -> logger, "Hubo un error aceptando la conexión");
            continue;
        }
        config_current_process -> current_process -> conection_module_console = *console_socket;
        log_debug(config_current_process -> global_config_kernel -> logger, "Se aceptó una conexión en el socket %d", *console_socket);
        op_code_reception result;
        //Cuando me llega una nueva consola lo mando a un recividor de nuevo pcb;
        pthread_create(&thread, NULL,  (void*) reciver_new_pcb, config_current_process);
        pthread_join(thread, (void**) &result);
		//socket_send_message(result, console_socket);
        free(console_socket);
         // hacer algo con el valor devuelto por long_term_scheduler
    }
}
//TODO: HACER MAS BONITO
void listen_cpu(config_current_process* config_current_process) {
    while (1) {
		config_current_process -> current_process -> pcb = socket_receive_package(config_current_process -> global_config_kernel -> conection_module_cpu);
        pthread_t thread;
        pthread_create(&thread, NULL, planificador_kernel, (void*) config_current_process -> current_process);
		pthread_detach(thread);
    }
}

t_pcb* create_pcb(process* process,t_log* logger, int default_burst_time) {
	t_log* log = logger;
	t_pcb* pcb = malloc(sizeof(t_pcb));
	pcb->state_pcb = NEW;
	pcb->pid = 2000 + process -> conection_module_console;
	pcb->aprox_burst_time = default_burst_time;
	pcb->execution_context = malloc(sizeof(execution_context));
	pcb->execution_context->instructions = queue_create();
	pcb->execution_context->program_counter = 0;
	log_info(log, "Proceso %d creado en NEW", pcb->pid);
	handle_incoming_instructions(pcb->execution_context->instructions, process->conection_module_console, logger);
	free(process);
	return pcb;
}

int aprox_burst_time(int value_aprox_time) {
	return value_aprox_time;
}