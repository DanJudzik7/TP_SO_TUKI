#include "consola.h"

int main(int argc, char** argv) {
	// Inicia el logger
	t_log* logger = start_logger("consola");
	// Carga archivo de configuración
	t_config* config = start_config("consola");

	// Inicia conexión con kernel
	char* ip = config_get_string_value(config, "IP_KERNEL");
	char* port = config_get_string_value(config, "PUERTO_KERNEL");
	int socket_kernel = socket_initialize(ip, port);
	if (socket_kernel == -1) {
		log_error(logger, "No se pudo conectar a kernel con %s:%s", ip, port);
		exit(EXIT_FAILURE);
	}
	log_warning(logger, "Conectado a kernel en %s:%s", ip, port);

	// Manda las instrucciones hardcodeadas
	char* mensaje = "Consola inicializada";
	socket_send_message(mensaje, socket_kernel);

	// Leemos el archivo y lo mandamos a un handler instruction que maneja que hacer con lo leído
	process_instructions_from_file(socket_kernel);
	char* mensaje_ok = "OK_SEND_INSTRUCTIONS";
	// Migrar a instrucciones en un mismo paquete, ara evitar problemas red y otros
	socket_send_message(mensaje_ok, socket_kernel);
	socket_receive_message(socket_kernel);
	socket_receive_message(socket_kernel); // Arreglar el bug que hace que esto esté dos veces
	log_info(logger, "Instrucciones cargadas enviadas al kernel");

	log_info(logger, "Ahora pasando a consola interactiva...");
    char* line;
	while (1) { // Cambiar esto a 0 para desactivar la consola interactiva
        line = readline("> ");
        if (!line) continue;
        if (line) add_history(line);                        
        if (!strncmp(line, ".exit", 4)) {
			log_warning(logger, "Saliendo");
			break;
		}
        if (socket_send_package(package_instruction_build(line), socket_kernel) == -1) break;
		if (socket_send_message(mensaje_ok, socket_kernel) == -1) break;
        socket_receive_message(socket_kernel);
        socket_receive_message(socket_kernel);
        // Esto me parece que se puede borrar
		/*
		read(socket_kernel, buffer, 1024);
        op_code_reception codop_console_recived = opcode_receive(buffer);
		if (codop_console_recived == OK) {
			log_info(logger, "Recibimos un OK por parte del kernel \n");
		}
		if (codop_console_recived == ERROR) {
			log_info(logger, "Recibimos un ERROR por parte del kernel \n");
		} */
	}
	socket_close(socket_kernel);
	log_warning(logger, "Consola desconectada de kernel");
	free(line);
	return 0;
}