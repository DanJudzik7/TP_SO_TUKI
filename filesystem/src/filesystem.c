#include "filesystem.h"

extern config_filesystem config_fs;

int main(int argc, char** argv) {
    setup_config();
	
    char* port = config_get_string_value(config_fs.config, "PUERTO_ESCUCHA");
    int server_socket = socket_initialize_server(port); // Inicializo el socket en el puerto cargado por la config
	log_warning(config_fs.logger, "Socket %d de servidor inicializado en puerto %s", server_socket, port);

	while (1) {
		pthread_t thread;
		int kernel_socket = socket_accept(server_socket);
		if (kernel_socket == -1) {
			log_warning(config_fs.logger, "Hubo un error aceptando la conexión");
			continue;
		}
		pthread_create(&thread, NULL, (void*)handle_kernel, &kernel_socket);
		pthread_join(thread, NULL);
	}
}