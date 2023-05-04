#include "handler_instruction_kernel.h"

int instruction_handler_kernel() {
	while (1) {
		int cod_op = 0;	 // REVISAR
		switch (cod_op) {
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
			case (-1):
				printf("Error al recibir el código de operación\n");
				return (-1);
		}
	}
}

void handle_incoming_instructions(t_queue* queue, int socket_console, t_log* logger) {
	int cod_op = 0;
	char* msg_invalid = "Instrucciones inválidas recibidas";
	char* msg_valid = "Instrucciones recibidas por el kernel";
	while (1) {
		// Seguir mejorando esto
		cod_op = socket_receive_operation(socket_console);
		if (cod_op == (-1)) {
			// La siguiente línea se va a optimizar al mejorar paquetes con mensajes
			socket_receive_message(socket_console);
			if (socket_send_message(msg_invalid, socket_console) == -1) break;
			log_debug(logger, msg_invalid);
			continue;
		}
		log_debug(logger, "Código de operación recibido: %i", cod_op);
		if (cod_op == MENSAJE) {
			if (socket_receive_message(socket_console) == 1) {
				// Si recibe el mensaje de finalización de instrucciones, avisa que las recibió y las devuelve en forma de lista
				socket_send_message(msg_valid, socket_console);
			}
		} else {
			queue_push(queue, socket_receive_package(socket_console));
			break;
		}
	}
}

// debemos hacer que cada vez que recibimos un payload,
// esto se encarga de crear una cola, y va cargando una lista de instrucciones [ ["AUX", Arg1, Arg2] , [] ]
// Para dps si lo enviamos a la cpu podemos saber el tamaño de cada arg de la lista :D
void list_instructions(char* instructions) {
}