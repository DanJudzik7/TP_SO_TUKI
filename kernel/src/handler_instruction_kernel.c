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
	int op_code = 0;
	char* welcome_message = "Instrucciones recibidas por el kernel. Tu PID es: %i";
	char generated_message[65];
	while (1) {
		if (pcb->state == EXIT_PROCESS) break;
		// Seguir mejorando esto
		op_code = socket_receive_operation(pcb->pid);
		if (op_code == -1) {
			// La siguiente línea se va a optimizar al mejorar paquetes con mensajes
			socket_receive_message(pcb->pid);
			if (socket_send_message("Instrucciones inválidas recibidas", pcb->pid) == -1) {
				pcb->state = EXIT_PROCESS;
				break;
			}
			printf("Instrucciones inválidas recibidas\n");
			continue;
		} else if (op_code == 32767) { // Cliente desconectado
			pcb->state = EXIT_PROCESS;
			printf("El cliente se desconectó\n");
			break;
		}
		printf("Código de operación recibido: %i\n", op_code);
		if (op_code == MENSAJE) {
			if (socket_receive_message(pcb->pid) == 1) {
				// Si recibe el mensaje de finalización de instrucciones, avisa que las recibió y le devuelve el PID 
				sprintf(generated_message, welcome_message, pcb->pid);
				socket_send_message(generated_message, pcb->pid);
			}
		} else {
			queue_push(pcb->execution_context->instructions, socket_receive_package(pcb->pid));
			pcb->state = READY;
		}
	}
	socket_close(pcb->pid);
}

// To do: Migrar a que las instrucciones se manden en un solo paquete
// esto se encarga de crear una cola, y va cargando una lista de instrucciones [ ["AUX", Arg1, Arg2] , [] ]
// Para dps si lo enviamos a la cpu podemos saber el tamaño de cada arg de la lista :D