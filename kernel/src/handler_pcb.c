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
	log_info(hcp->gck->logger, "El proceso %d se creó en NEW", pcb->pid);
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
		log_error(hcp->gck->logger, "El cliente se desconectó");
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

void handle_fs(t_helper_file_instruction* hfi) {
	while (1) {
		while (queue_is_empty(hfi->file_instructions)) sleep(1);
		t_instruction* fs_op = queue_pop(hfi->file_instructions);
		if (fs_op->op_code != F_READ && fs_op->op_code != F_WRITE) {
			log_error(hfi->logger, "Operación inválida recibida");
			continue;
		}
		if (!socket_send(hfi->socket_filesystem, serialize_instruction(fs_op))) {
			log_error(hfi->logger, "Error al enviar operación a filesystem");
			continue;
		}
		t_package* package = socket_receive(hfi->socket_filesystem);
		printf("La %s del archivo fue %s", fs_op->op_code == F_READ ? "lectura" : "escritura", package->type == MESSAGE_OK ? "exitosa" : "fallida");
		package_destroy(package);
		instruction_destroy(fs_op);
	}
}