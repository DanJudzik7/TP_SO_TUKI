#include "filesystem.h"

int main(int argc, char** argv) {
	t_log* logger = start_logger("filesystem");
	t_config* config = start_config("filesystem");
	log_warning(logger, "Iniciando el filesystem");

	char* port = config_get_string_value(config, "PUERTO_ESCUCHA");	 // Obtenemos el puerto con el que escucharemos conexiones
	int socket_fs = socket_initialize_server(port); // Inicializo el socket en el puerto cargado por la config
}