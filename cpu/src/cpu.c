#include "cpu.h"

configuration_cpu config_cpu;

int main(int argc, char** argv) {
	sem_init(&(config_cpu.flag_running), 0, 1);
	sem_init(&(config_cpu.flag_dislodge), 0, 1);

	t_log* logger = start_logger("cpu");
	t_config* config = start_config("cpu");

	log_warning(logger, "Iniciando la CPU");
	config_cpu.logger = logger;

	char* port = config_get_string_value(config, "PUERTO_ESCUCHA");
	int socket_cpu = socket_initialize_server(port);  // Inicializo el socket en el puerto cargado por la config
	if (socket_cpu == -1) {
		log_error(logger, "No se pudo inicializar el socket de servidor");
		exit(EXIT_FAILURE);
	}
	log_warning(logger, "Socket de servidor inicializado en puerto %s", port);

	// int conn_memoria = connect_module(config, logger, "MEMORIA");

	// int kernel_fd = receive_modules(logger, config);
	// execution_context* context = create_context_test();
	// TODO: En el primer recv llega basura no se porque
	// socket_receive_message(kernel_fd);
	// socket_receive_message(kernel_fd);

	// To do: CPU debe recibir solo execution context, no todo este PCB

	// log_info(logger, "El proceso %d se creó en NEW\n", pcb_test->pid);
	// pcb_test->execution_context = context;

	pthread_t thread_consola;
	pthread_create(&thread_consola, NULL, (void*)listen_kernel, &socket_cpu);
	pthread_join(thread_consola, NULL);

	// Recibe los pcbs que aca están harcodeados y los opera
	// int kernel_socket = socket_accept(socket_cpu);
	// config_cpu.connection_kernel = kernel_socket;
	// fetch(context);

	// free(context);
	// free(pcb_test);
}

void listen_kernel(int* socket_cpu) {
	int sem_value;

	while (1) {
		int kernel_socket = socket_accept(*socket_cpu);
		config_cpu.connection_kernel = kernel_socket;
		sem_getvalue(&config_cpu.flag_running, &sem_value);
		if (sem_value==1) {
			sem_wait(&config_cpu.flag_running);
			t_package* package = socket_receive(kernel_socket);
			if (package == NULL) {
				// Definir si acá se tiene que hacer algo más
				log_error(config_cpu.logger, "El kernel se desconectó");
				break;
			}
			if (package->type != EXECUTION_CONTEXT) {
				char* invalid_package = string_from_format("Paquete inválido recibido: %i\n", package->type);
				socket_send(kernel_socket, serialize_message(invalid_package, true));
				log_warning(config_cpu.logger, "Se recibió un paquete inválido: %i", package->type);
				free(invalid_package);
				package_destroy(package);
				continue;
			}
			log_info(config_cpu.logger, "Llegó un nuevo Execution Context");
			execution_context* context = deserialize_execution_context(package);
			
			pthread_t thread;
			// Se crea un thread para ejecutar el contexto y sus instrucciones
			pthread_create(&thread, NULL, (void*)fetch, context);
			log_warning(config_cpu.logger, "Hilo de ejecución creado");
			pthread_join(thread, NULL);
			sem_post(&config_cpu.flag_running);
		} else {
			// En caso contrario se envía un mensaje al kernel de que estoy ocupado
			log_info(config_cpu.logger, "La CPU está ocupada");
			t_package* package = package_new(MESSAGE_BUSY);
			socket_send(kernel_socket, package);
		}
	}
	log_warning(config_cpu.logger, "Se cerró el socket de conexión con el kernel");
}