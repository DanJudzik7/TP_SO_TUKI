#include "handler_pcb.h"

void listen_consoles(t_global_config_kernel* gck) {
	while (1) {
		pthread_t thread;
		int console_socket = socket_accept(gck->server_socket);
		if (console_socket == -1) {
			log_warning(gck->logger, "Hubo un error aceptando la conexión");
			continue;
		}
		helper_create_pcb* hcp = s_malloc(sizeof(helper_create_pcb));
		hcp->connection = console_socket;
		hcp->gck = gck;
		// Se crea un thread para escuchar las instrucciones
		pthread_create(&thread, NULL, (void*)handle_new_process, hcp);
		pthread_detach(thread);
	}
}

void handle_new_process(helper_create_pcb* hcp) {
	t_pcb* pcb = pcb_new(hcp->connection, hcp->gck->default_burst_time);
	log_info(hcp->gck->logger, "Se crea el proceso %d en NEW", pcb->pid);
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
		log_error(hcp->gck->logger, "La consola en el socket %d se desconectó", pcb->pid);
		return;
	}

	if (package->type != INSTRUCTIONS) {
		char* invalid_package = string_from_format("Paquete inválido recibido: %i\n", package->type);
		if (!socket_send(pcb->pid, serialize_message(invalid_package, true))) {
			free(invalid_package);
			pcb_destroy(pcb);
			package_destroy(package);
			return;
		}
		free(invalid_package);
	}

	if (!socket_send(pcb->pid, serialize_message("Instrucciones recibidas", false))) {
		pcb_destroy(pcb);
		package_destroy(package);
		return;
	}

	deserialize_instructions(package, pcb->execution_context->instructions);
	queue_push(hcp->gck->new_pcbs, pcb);
	package_destroy(package);

	long_term_schedule(hcp->gck);
}

void handle_fs(t_helper_fs_handler* hfi) {
	while (1) {
		while (queue_is_empty(hfi->file_instructions)) sleep(1);
		t_file_instruction* fi = queue_pop(hfi->file_instructions);
		if (fi->instruction->op_code != F_READ && fi->instruction->op_code != F_WRITE) {
			log_error(hfi->logger, "Operación inválida recibida");
			continue;
		}
		if (!socket_send(hfi->socket_filesystem, serialize_instruction(fi->instruction))) {
			log_error(hfi->logger, "Error al enviar operación a filesystem");
			continue;
		}
		t_package* package = socket_receive(hfi->socket_filesystem);
		if (package == NULL || package->type != MESSAGE_OK) {
			log_error(hfi->logger, "Error al %s del archivo", fi->instruction->op_code == F_READ ? "leer" : "escribir");
			continue;
		}
		log_warning(hfi->logger, "PID: %d - %s Archivo: %s - Puntero: %s - Dirección Memoria: %s - Tamaño: %s",
					fi->pcb->pid,
					fi->instruction->op_code == F_READ ? "Leer" : "Escribir",
					(char*)list_get(fi->instruction->args, 0),
					(char*)list_get(fi->instruction->args, 1),
					(char*)list_get(fi->instruction->args, 6),
					(char*)list_get(fi->instruction->args, 2));
		fi->pcb->state = READY;
		log_warning(hfi->logger, "PID: %d - Estado Anterior: BLOCK - Estado Actual: READY", fi->pcb->pid);
		package_destroy(package);
		instruction_destroy(fi->instruction);
		free(fi);
	}
}