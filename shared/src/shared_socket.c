#include "shared_socket.h"

int socket_initialize(char* ip, char* port) {
	struct addrinfo hints, *server_info;
	memset(&hints, 0, sizeof(hints));
	hints.ai_family = AF_UNSPEC;
	hints.ai_socktype = SOCK_STREAM;
	hints.ai_flags = AI_PASSIVE;
	getaddrinfo(ip, port, &hints, &server_info);

	// Creamos el socket de escucha del servidor
	int server_socket = socket(server_info->ai_family, server_info->ai_socktype, server_info->ai_protocol);
	int yes = 1;
	setsockopt(server_socket, SOL_SOCKET, SO_REUSEADDR, &yes, sizeof yes);
	if (server_socket == -1) return -1;

	if (ip == NULL) { // Para servidor
		// Asociamos el socket a un port
		bind(server_socket, server_info->ai_addr, server_info->ai_addrlen);
		// Escuchamos las conexiones entrantes
		if(listen(server_socket, SOMAXCONN) == -1) return -1;
	} else { // Para cliente
		// Ahora que tenemos el socket, vamos a conectarlo
		if (connect(server_socket, server_info->ai_addr, server_info->ai_addrlen) == -1) return -1;
	}

	freeaddrinfo(server_info);
	return server_socket;
}

int socket_initialize_server(char* port) {
	return socket_initialize(NULL, port);
}

int socket_accept(int server_socket) {
	int target_socket = accept(server_socket, NULL, NULL);
	if (target_socket == -1) printf("Error de conexión al servidor\n");
	return target_socket;
}

bool socket_send(int target_socket, t_package* package) {
	serialize_package(package);
	bool send_ret = send(target_socket, package->buffer, package->size, MSG_NOSIGNAL) != -1;
	package_destroy(package);
	return send_ret;
}

t_package* socket_receive(int target_socket) {
	t_package* package = s_malloc(sizeof(t_package));
	// Recibe un número unsigned long del cliente
	uint64_t* size = s_malloc(sizeof(uint64_t));
	if (recv(target_socket, size, sizeof(uint64_t), MSG_WAITALL) < 1) {
		free(size);
		free(package);
		return NULL;
	}
	package->size = *size;
	free(size);
	// Recibe un número int del cliente
	int32_t* type = s_malloc(sizeof(int32_t));
	if (recv(target_socket, type, sizeof(int32_t), MSG_WAITALL) < 1) {
		free(type);
		free(package);
		return NULL;
	}
	package->type = *type;
	free(type);
	// Recibe el buffer del cliente
	package->buffer = s_malloc(package->size);
	if (recv(target_socket, package->buffer, package->size, MSG_WAITALL) < 1) {
		free(package->buffer);
		free(package);
		return NULL;
	}
	return package;
}

void socket_close(int target_socket) {
	close(target_socket);
}

int connect_module(t_config* config, t_log* logger, char* module) {
	char* port = string_from_format("PUERTO_%s", module);
	char* ip = string_from_format("IP_%s", module);
	char* module_port = config_get_string_value(config, port);
	char* module_ip = config_get_string_value(config, ip);
	int module_socket = socket_initialize(module_ip, module_port);
	free(port);
	free(ip);
	free(module_port);
	free(module_ip);
	/*if (!socket_send(module_socket, package_new_message("Mensaje de prueba", false))) {
		printf("Error al enviar mensaje de prueba en %s en %d\n", module, module_socket);
		return -1;
	}*/
	if (module_socket == -1) {
		log_error(logger, "No se pudo conectar a módulo %s", module);
		exit(EXIT_FAILURE);
	}
	log_info(logger, "Conectado a módulo %s en socket %d", module, module_socket);
	return module_socket;
}

int receive_module(t_log* logger, t_config* config) {
	// Obtiene el puerto con el que escucharemos conexiones
	char* port = config_get_string_value(config, "PUERTO_ESCUCHA");
	log_info(logger, "El value del port es %s \n", port);

	// Inicializo el socket en el puerto cargado por la config
	int server_socket = socket_initialize_server(port);
	log_info(logger, "SOCKET INICIALIZADO");

	// Acepto la primera conexión entrante
	return socket_accept(server_socket);
}