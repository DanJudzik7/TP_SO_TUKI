#include "consola.h"

int main(int argc, char** argv) {
	t_log* logger = start_logger("consola");	 // Inicia el logger
	t_config* config = start_config("consola");	 // Carga archivo de configuración

	// Inicia conexión con kernel
	char* ip = config_get_string_value(config, "IP_KERNEL");
	char* port = config_get_string_value(config, "PUERTO_KERNEL");
	int socket_kernel = socket_initialize(ip, port);
	if (socket_kernel == -1) {
		log_error(logger, "No se pudo conectar a kernel con %s:%s", ip, port);
		return 1;
	}
	log_warning(logger, "Conectado a kernel en %s:%s", ip, port);

	t_package* init_package = socket_receive(socket_kernel);

	if (init_package == NULL) {
		log_error(logger, "Se desconectó el kernel");
		return 1;
	} else if (init_package->type == MESSAGE_OK) {
		char* message = deserialize_message(init_package);
		log_info(logger, "< %s", message);
		free(message);
	} else if (init_package->type == MESSAGE_FLAW) {
		char* message = deserialize_message(init_package);
		log_warning(logger, "< %s", message);
		free(message);
	} else {
		log_warning(logger, "Paquete inválido recibido");
		return 1;
	}

	if (!socket_send(socket_kernel, process_instructions(argv[1] != NULL ? argv[1] : "instrucciones"))) {
		log_error(logger, "Error al precargar instrucciones al kernel");
		return 1;
	} else
		log_info(logger, "Instrucciones precargadas enviadas al kernel");
	log_info(logger, "Esperamos mensajes por parte del kernel");

	// Esperamos a que el kernel nos envíe que termino de ejecutar todas las instrucciones
	while (1) {
		t_package* package = socket_receive(socket_kernel);
		if (package == NULL) break;
		if (package->type == MESSAGE_DONE) {
			log_warning(logger, "Ejecución finalizada. Saliendo...");
			package_destroy(package);
			break;
		} else if (package->type == MESSAGE_OK) {
			char* message = deserialize_message(package);
			log_info(logger, "< %s", message);
			free(message);
		} else if (package->type == MESSAGE_FLAW) {
			char* message = deserialize_message(package);
			log_warning(logger, "< %s", message);
			free(message);
		} else {
			log_warning(logger, "Paquete inválido recibido");
			package_destroy(package);
			break;
		}
	}
	log_warning(logger, "El kernel se ha desconectado de esta consola");
	log_destroy(logger);
	config_destroy(config);
	return 0;
}