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

	t_package* package = socket_receive(socket_kernel);

	if (package->type == MESSAGE_OK) log_info(logger, "< %s", deserialize_message(package));
	else if (package->type == MESSAGE_FLAW) log_warning(logger, "< %s", deserialize_message(package));
	else log_warning(logger, "Paquete inválido recibido");


	if (!socket_send(socket_kernel, process_instructions(socket_kernel))) {
		log_error(logger, "Error al precargar instrucciones al kernel");
	}
	else log_info(logger, "Instrucciones precargadas enviadas al kernel");
	log_info(logger, "Esperamos mensajes por parte del kernel");
	//Esperamos a que el kernel nos envie que termino de ejecutar todas las instrucciones
	// Leemos el archivo y lo mandamos a un handler instruction que maneja que hacer con lo leído
	//bool interactive = false;
    //char* line;
	while (1) {
		t_package* package = socket_receive(socket_kernel); 
		if (package == NULL) {
			continue;
		}
		if (package->type == MESSAGE_DONE) {
			log_warning(logger, "Ejecución finalizada. Saliendo...");
			package_destroy(package);
			return 0;
		} else if (package->type == MESSAGE_OK) {
			log_info(logger, "< %s", deserialize_message(package));
		} else if (package->type == MESSAGE_FLAW) {
			log_warning(logger, "< %s", deserialize_message(package));
		} else {
			log_warning(logger, "Paquete inválido recibido");
			break;
		}
	}	
	package_destroy(package);
	log_warning(logger, "El kernel se ha desconectado de esta consola");
	return 0;
}

//Guardado por si las dudas pero no creo que sea necesario
/*
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
		package_nest(package, parse_instruction(line));
		if (!socket_send(socket_kernel, package)) break;
		}
		else {
		if (!socket_send(socket_kernel, process_instructions(socket_kernel))) break;
			log_info(logger, "Instrucciones precargadas enviadas al kernel");
			interactive = true;
	}*/