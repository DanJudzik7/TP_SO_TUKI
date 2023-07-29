#include "cpu.h"

configuration_cpu config_cpu;

int main(int argc, char** argv) {
	t_log* logger = start_logger("cpu");
	char* config_path = argv[1];
	t_config* config = start_config(config_path);

	log_warning(logger, "Iniciando la CPU");
	config_cpu.logger = logger;
	config_cpu.max_segment_size = config_get_int_value(config, "TAM_MAX_SEGMENTO");
	config_cpu.instruction_delay = config_get_int_value(config, "RETARDO_INSTRUCCION");
	char* port = config_get_string_value(config, "PUERTO_ESCUCHA");
	int server_socket = socket_initialize_server(port);
	if (server_socket == -1) {
		log_error(logger, "No se pudo inicializar el socket de servidor");
		exit(EXIT_FAILURE);
	}
	log_warning(logger, "Socket de servidor inicializado en puerto %s", port);

	config_cpu.socket_memory = connect_module(config, logger, "MEMORIA");

	while (1) {
		int kernel_socket = socket_accept(server_socket);
		if (kernel_socket == -1) {
			log_warning(config_cpu.logger, "Hubo un error aceptando la conexión");
			continue;
		}
		while (1) {
			t_package* package = socket_receive(kernel_socket);
			if (package != NULL && package->type != EXECUTION_CONTEXT) {
				char* invalid_package = string_from_format("Paquete inválido recibido: %i\n", package->type);
				socket_send(kernel_socket, serialize_message(invalid_package, true));
				log_warning(config_cpu.logger, "Se recibió un paquete inválido: %i", package->type);
				free(invalid_package);
				package_destroy(package);
				continue;
			}
			if (package == NULL) {
				log_error(config_cpu.logger, "El kernel se desconectó");
				break;
			}
			t_execution_context* context = deserialize_execution_context(package);
			context->kernel_request = NULL;
			log_info(config_cpu.logger, "Llegó un nuevo Execution Context, se va a ejecutar desde la instrucción %d", context->program_counter);
			//Inicializo el cronometro del tiempo de ráfaga
			config_cpu.burst_time = temporal_create();
			// Ejecuto mientras no se tenga que desalojar
			while (context->kernel_request == NULL) {
				t_instruction* instruction = fetch(context);
				if (instruction == NULL) {
					context->kernel_request = instruction_new(EXIT);
					break;
				}
				t_physical_address* pa = decode(instruction, context);
				execute(instruction, context, pa);
				free(pa);
				context->program_counter++;
			}
			//Detengo el cronometro de ráfaga
			temporal_stop(config_cpu.burst_time);
			//Obtengo su tiempo
			context->last_burst_time = temporal_gettime(config_cpu.burst_time);
			//Lo destruyo para liberar memoria
			temporal_destroy(config_cpu.burst_time);
			t_package* package_context = serialize_execution_context(context);
			if (!socket_send(kernel_socket, package_context)) {
				log_error(config_cpu.logger, "Error al enviar el Execution Context al Kernel");
				break;
			}
			log_info(config_cpu.logger, "Se desalojó y se envió el Execution Context al Kernel");
			execution_context_destroy(context);
		}
		log_warning(config_cpu.logger, "Se desconectó de kernel, abriendo para nuevas conexiones");
	}
	log_warning(config_cpu.logger, "CPU ha finalizado su ejecución");
	log_destroy(logger);
	config_destroy(config);
}