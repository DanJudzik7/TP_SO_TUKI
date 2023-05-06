#include "handler_instruction_memoria.h"

int instruction_handler_memoria(int cliente_fd) {
	t_list* payload;
	t_package* package;

	while (1) {
		int cod_op = socket_receive_operation(cliente_fd);
		printf("El código de op es: %i\n", cod_op);

		switch (cod_op) {
			case CREATE_SEGMENT:
				printf("RECIBIMOS UNA INSTRUCCIÓN DE CREAR SEGMENTO DE MEMORIA\n");
				op_code_reception result_creation_segment = create_memory_segment();
				package = package_create(result_creation_segment);
				socket_send_package(package, cliente_fd);

				break;
			case DELETE_SEGMENT:
				printf("RECIBIMOS UNA INSTRUCCIÓN DE ELIMINAR UN SEGMENTO DE MEMORIA\n");
				payload = socket_receive_package(cliente_fd);
				// deserialize_payload(payload);
				op_code_reception result_delete_segment = delete_memory_segment(0);	 // ACTUALMENTE PASO UN ID = 0, PERO LUEGO NECESITO SABERLO DADO EL
				package = package_create(result_delete_segment);
				socket_send_package(package, cliente_fd);

				break;
			default:
				printf("Error al recibir código de operación\n");
				return -1;
		}
	}
}