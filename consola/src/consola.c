#include "consola.h"

int main(int argc, char** argv) {
	t_log* logger = start_logger("consola"); // Inicia el logger
	t_config* config = start_config("consola"); // Carga archivo de configuración

	// Inicia conexión con kernel
	char* ip = config_get_string_value(config, "IP_KERNEL");
	char* port = config_get_string_value(config, "PUERTO_KERNEL");
	int socket_kernel = socket_initialize(ip, port);
	if (socket_kernel == -1) {
		log_error(logger, "No se pudo conectar a kernel con %s:%s", ip, port);
		exit(EXIT_FAILURE);
	}
	log_warning(logger, "Conectado a kernel en %s:%s", ip, port);

	// Leemos el archivo y lo mandamos a un handler instruction que maneja que hacer con lo leído
	bool interactive = false;
    char* line;
	while (1) {
		t_package* package = socket_receive(socket_kernel);
		if (package == NULL) {
			log_warning(logger, "Error al recibir respuesta");
			break;
		}
		if (package->field == MESSAGE_OK) log_info(logger, "< %s", deserialize_message(package));
		else if (package->field == MESSAGE_FLAW) log_warning(logger, "< %s", deserialize_message(package));
		else log_warning(logger, "Paquete inválido recibido\n");
		if (interactive) {
			//break; // Activar esta línea para desactivar la consola interactiva
			line = readline("> ");
			if (!line) continue;
			if (line) add_history(line);                        
			if (!strncmp(line, ".exit", 4)) {
				log_warning(logger, "Saliendo");
				break;
			}
			package = package_new(INSTRUCTIONS);
			package_nest(package, serialize_instruction(line));
			if (!socket_send(socket_kernel, package)) break;
		} else {
			if (!socket_send(socket_kernel, process_instructions(socket_kernel))) break;
			log_info(logger, "Instrucciones precargadas enviadas al kernel");
			interactive = true;
		}
	}
	socket_close(socket_kernel);
	log_warning(logger, "Consola desconectada de kernel");
	free(line);
	return 0;
}