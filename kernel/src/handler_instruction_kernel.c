#include "handler_instruction_kernel.h"

int instruction_handler_kernel() {
	while (1) {
		int op_code = 0;	 // REVISAR
		switch (op_code) {
			case F_READ:
			case F_WRITE:
				break;
			case SET:
			case MOV_IN:
			case MOV_OUT:
			case F_TRUNCATE:
			case F_SEEK:
			case CREATE_SEGMENT:
				break;
			case I_O:
			case WAIT:
			case SIGNAL:
			case F_OPEN:
			case F_CLOSE:
			case DELETE_SEGMENT:
				break;
			case EXIT:
			case YIELD:
				break;
			case -1:
				printf("Error al recibir el código de operación\n");
				return -1;
		}
	}
}

void handle_incoming_instructions(t_pcb* pcb) {
	printf("Nueva consola conectada. PID: %i\n", pcb->pid);
	char* welcome_message = string_from_format("Bienvenido al kernel. Tu PID es: %i", pcb->pid);
	if (!socket_send_message(pcb->pid, welcome_message, true)) pcb->state = EXIT_PROCESS;
	free(welcome_message);
	while (1) {
		if (pcb->state == EXIT_PROCESS) break;
		t_package* package = socket_receive(pcb->pid);
		if (package == NULL) {
			pcb->state = EXIT_PROCESS;
			printf("El cliente se desconectó\n");
			break;
		}
		if (package->type == MESSAGE_OK || package->type == MESSAGE_FLAW) {
			socket_send_message(pcb->pid, "Mensaje recibido", false);
			char* message = deserialize_message(package);
			printf("< %s\n", message);
			free(message);
		} else if (package->type == INSTRUCTIONS) {
			socket_send_message(pcb->pid, "Instrucciones recibidas", false);
			deserialize_instructions(package, pcb->execution_context->instructions);
			pcb->state = READY;
		} else {
			char* invalid_package = string_from_format("Paquete inválido recibido: %i\n", package->type);
			socket_send_message(pcb->pid, invalid_package, true);
			free(invalid_package);
		}
		package_destroy(package);
	}
	socket_close(pcb->pid);
}