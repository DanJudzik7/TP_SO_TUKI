#include "handler_pcb.h"

void listen_consoles(t_global_config_kernel* gck) {
	while (1) {
		pthread_t thread;
		int console_socket = socket_accept(gck->connection_kernel);
		if (console_socket == -1) {
			log_warning(gck->logger, "Hubo un error aceptando la conexión");
			continue;
		}
		helper_create_pcb *hcp = s_malloc(sizeof(helper_create_pcb));
		hcp->connection = console_socket;
		hcp->config = gck;
		// Se crea un thread para escuchar las instrucciones
		pthread_create(&thread, NULL, (void*)handle_incoming_instructions, hcp);
		pthread_detach(thread);
	}
}

void handle_incoming_instructions(helper_create_pcb* hcp) {
    t_pcb* pcb = pcb_new(hcp->connection, hcp->config->default_burst_time);
    printf("Nueva consola conectada. PID: %i\n", pcb->pid);
    log_info(hcp->config->logger, "El proceso %d se creó en NEW", pcb->pid);
    char* welcome_message = string_from_format("Bienvenido al kernel. Tu PID es: %i", pcb->pid);
    if (!socket_send(pcb->pid, serialize_message(welcome_message, true))) {
        pcb_destroy(pcb);
        free(welcome_message);
        return;
    }
    free(welcome_message);
	
    t_package* package = socket_receive(pcb->pid);
    if (package == NULL) {
        pcb_destroy(pcb);
        printf("El cliente se desconectó\n");
        return;
    }

    if (package->type == INSTRUCTIONS) {
        if (!socket_send(pcb->pid, serialize_message("Instrucciones recibidas", false))) {
            pcb_destroy(pcb);
            package_destroy(package);
            return;
        }
        
        deserialize_instructions(package, pcb->execution_context->instructions);
        queue_push(hcp->config->new_pcbs, pcb);
	 package_destroy(package);
    } else {
        char* invalid_package = string_from_format("Paquete inválido recibido: %i\n", package->type);
        if (!socket_send(pcb->pid, serialize_message(invalid_package, true))) {
            free(invalid_package);
            pcb_destroy(pcb);
            package_destroy(package);
            return;
        }
        free(invalid_package);
    }

    long_term_schedule(hcp->config);
}


t_pcb* pcb_new(int pid, int burst_time) {
	// La inicialización se hace de forma segura en memoria (con memset)
	t_pcb* pcb = s_malloc(sizeof(t_pcb));
	pcb->state = NEW;
	pcb->pid = pid;
	pcb->aprox_burst_time = burst_time;
	pcb->last_ready_time = time(NULL);
	pcb->files = list_create();
	pcb->execution_context = s_malloc(sizeof(execution_context));
	pcb->execution_context->instructions = queue_create();
	pcb->execution_context->program_counter = 0;
	pcb->execution_context->updated_state = NEW;
	pcb->execution_context->cpu_register = s_malloc(sizeof(cpu_register));
	memset(pcb->execution_context->cpu_register, 0, sizeof(cpu_register));
	pcb->execution_context->segment_table = s_malloc(sizeof(segment_table));
	memset(pcb->execution_context->segment_table, 0, sizeof(segment_table));
	return pcb;
}

void pcb_destroy(t_pcb* pcb) {
	list_destroy_and_destroy_elements(pcb->files, (void*)delete_file);
	queue_destroy_and_destroy_elements(pcb->execution_context->instructions, (void*)delete_instruction);
	free(pcb->execution_context->cpu_register);
	pcb->execution_context->cpu_register = NULL;
	free(pcb->execution_context->segment_table);
	pcb->execution_context->segment_table = NULL;
	free(pcb->execution_context);
	pcb->execution_context = NULL;
	free(pcb);
	pcb = NULL;
}

void delete_file(t_file* file) {
	free(file->file_direction);
	free(file);
}

void delete_instruction(t_instruction* instruction) {
	list_destroy_and_destroy_elements(instruction->args, free);
	free(instruction);
}