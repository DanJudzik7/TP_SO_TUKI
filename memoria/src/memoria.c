#include "memoria.h"


int main() {
	initialize();
	log_info(config_memory.logger, "Iniciando Memoria...");
	int socket_memory = socket_initialize_server(config_memory.port);	 // Inicializo el socket en el puerto cargado por la config
	if (socket_memory == -1) {
		log_error(config_memory.logger, "No se pudo inicializar el socket de servidor");
		exit(EXIT_FAILURE);
	}
	log_warning(config_memory.logger, "Socket de servidor inicializado en puerto %s", config_memory.port);
}

