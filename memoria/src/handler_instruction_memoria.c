#include "handler_instruction_memoria.h"

int instruction_handler_memoria(int socket_client) {
	// Todo esto no está adaptado al nuevo packaging
	t_list* payload;
	t_package* package;

	while (1) {
		uint32_t* cod_op = socket_receive_int(socket_client);
		if (cod_op == NULL) {
			printf("Error al recibir código de operación\n");
			return -1;
		}
		printf("El código de operación es: %i\n", *cod_op);

		switch (*cod_op) {
			case CREATE_SEGMENT:
				printf("RECIBIMOS UNA INSTRUCCIÓN DE CREAR SEGMENTO DE MEMORIA\n");
				int result_creation_segment = create_memory_segment();
				package = package_new(result_creation_segment);
				if (!socket_send(socket_client, package)) {
					printf("Error al enviar el paquete\n");
					return -1;
				}

				break;
			case DELETE_SEGMENT:
				printf("RECIBIMOS UNA INSTRUCCIÓN DE ELIMINAR UN SEGMENTO DE MEMORIA\n");
				// payload = socket_receive(socket_client);
				//  deserialize_payload(payload);
				int result_delete_segment = delete_memory_segment(0);  // ACTUALMENTE PASO UN ID = 0, PERO LUEGO NECESITO SABERLO DADO EL
				package = package_new(result_delete_segment);
				if (!socket_send(socket_client, package)) {
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
}