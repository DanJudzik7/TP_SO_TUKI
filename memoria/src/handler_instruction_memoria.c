#include "handler_instruction_memoria.h"

int instruction_handler_memoria(int socket_client) {
	// Todo esto no está adaptado al nuevo packaging

	while (1) {
		t_package* package = socket_receive(socket_client);
		if (package == NULL) {
			printf("El cliente se desconectó\n");
			break;
		}
		printf("El código de operación es: %i\n", package->type);
		// To do: Mejorar lo que hace acá. Tiene que recibir un paquete de tipo INSTRUCTION, con el t_instruction nesteado adentro

		switch (package->type) {
			case CREATE_SEGMENT:
				printf("RECIBIMOS UNA INSTRUCCIÓN DE CREAR SEGMENTO DE MEMORIA\n");
				bool success_creation_segment = create_memory_segment();
				if (!socket_send(socket_client, serialize_message(success_creation_segment ? "Eliminado": "Error", false))) {
					printf("Error al enviar el paquete\n");
					return -1;
				}
				break;
			case DELETE_SEGMENT:
				printf("RECIBIMOS UNA INSTRUCCIÓN DE ELIMINAR UN SEGMENTO DE MEMORIA\n");
				bool success_delete_segment = delete_memory_segment(0);  // ACTUALMENTE PASO UN ID = 0, PERO LUEGO NECESITO SABERLO DADO EL
				if (!socket_send(socket_client, serialize_message(success_delete_segment ? "Eliminado": "Error", false))) {
					printf("Error al enviar el paquete\n");
					return -1;
				}
				break;
			case MESSAGE_OK:
				printf("RECIBIMOS UN HANDSAKE\n");
				break;
			default:
				printf("Error al recibir código de operación\n");
				return -1;
		}
	}
	return 0;
}