#include "cpu.h"

configuration_cpu config_cpu;

int main(int argc, char** argv) {
	sem_init(&(config_cpu.flag_dislodge), 0, 1);

	t_log* logger = start_logger("cpu");
	t_config* config = start_config("cpu");

	log_warning(logger, "Iniciando la CPU");
	config_cpu.logger = logger;
	config_cpu.max_segment_size = config_get_int_value(config, "TAM_MAX_SEGMENTO");
	config_cpu.instruction_delay = config_get_int_value(config, "RETARDO_INSTRUCCION");

	char* port = config_get_string_value(config, "PUERTO_ESCUCHA");
	int server_socket = socket_initialize_server(port);  // Inicializo el socket en el puerto cargado por la config
	if (server_socket == -1) {
		log_error(logger, "No se pudo inicializar el socket de servidor");
		exit(EXIT_FAILURE);
	}
	log_warning(logger, "Socket de servidor inicializado en puerto %s", port);

	config_cpu.socket_memory = connect_module(config, logger, "MEMORIA");

	// int kernel_fd = receive_modules(logger, config);
	// execution_context* context = create_context_test();
	// TODO: En el primer recv llega basura no se porque
	// socket_receive_message(kernel_fd);
	// socket_receive_message(kernel_fd);

	// To do: CPU debe recibir solo execution context, no todo este PCB

	// log_info(logger, "El proceso %d se creó en NEW\n", pcb_test->pid);
	// pcb_test->execution_context = context;

	// Recibe los pcbs que aca están harcodeados y los opera
	// fetch(context);

	// free(context);
	// free(pcb_test);

	while (1) {
		int kernel_socket = socket_accept(server_socket);
		if (kernel_socket == -1) {
			log_warning(config_cpu.logger, "Hubo un error aceptando la conexión");
			continue;
		}
		while (1) {
			t_package* package = socket_receive(kernel_socket);

			if (package->type != EXECUTION_CONTEXT && package != NULL) {
				char* invalid_package = string_from_format("Paquete inválido recibido: %i\n", package->type);
				socket_send(kernel_socket, serialize_message(invalid_package, true));
				log_warning(config_cpu.logger, "Se recibió un paquete inválido: %i", package->type);
				free(invalid_package);
				package_destroy(package);
				continue;
			}
			if (package == NULL) {
				// Definir si acá se tiene que hacer algo más
				log_error(config_cpu.logger, "El kernel se desconectó");
				break;
			}
			log_info(config_cpu.logger, "Llegó un nuevo Execution Context");
			execution_context* context = deserialize_execution_context(package);
			print_execution_context(context);
			bool flag_dislodge = false;
			t_instruction* instruction = NULL;
			// Ejecuto mientras el flag de desalojo este libre
			while (!flag_dislodge) {
				instruction = fetch(context);
				if (instruction == NULL) break;
				t_physical_address* pa = decode(context, instruction);
				execute(instruction, context, pa);
				context->program_counter++;
			}

			t_package* package_context = serialize_execution_context(context);
			if (!socket_send(kernel_socket, package_context)) {
				log_error(config_cpu.logger, "ERROR AL ENVIAR EL CONTEXT AL KERNEL");
				break;
			}
			log_info(config_cpu.logger, "Context enviado al Kernel");
		}
		log_warning(config_cpu.logger, "Se desconectó de kernel, abriendo para nuevas conexiones");
	}
}