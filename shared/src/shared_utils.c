#include "shared_utils.h"

char* deserialize_payload(t_list* payload) {
	return "IMPLEMENTAR";
}

t_config* start_config(char* process_name) {
	t_config* config = NULL;
	char* path_config;
	path_config = get_config_type(process_name, "config");
	config = config_create(path_config);
	if (config == NULL) {
		printf("Error al cargar la configuración\n");
		free(path_config);
		exit(EXIT_FAILURE);
	}
	free(path_config);
	return config;
}

t_log* start_logger(char* process_name) {
	t_log* logger;
	char* path_log;

	path_log = get_config_type(process_name, "log");

	if ((logger = log_create(path_log, process_name, true, LOG_LEVEL_INFO)) == NULL) {
		printf("Error al crear logger\n");
		exit(EXIT_FAILURE);
	} else {
		return logger;
	}
}

char* get_config_type(char* process_name, char* file_type) {
	char* directorio = getcwd(NULL, 0);

	if (directorio == NULL) {
		printf("Error al obtener el directorio actual\n");
		exit(EXIT_FAILURE);
	}

	char* ruta_config = malloc(strlen(directorio) + strlen("/cfg/.") + strlen(process_name) + strlen(".") + strlen(file_type) + 1);
	sprintf(ruta_config, "%s/cfg/%s.%s", directorio, process_name, file_type);

	if (ruta_config == NULL) {
		printf("Error al reservar memoria\n");
		free(directorio);
		exit(EXIT_FAILURE);
	} else {
		return ruta_config;
	}
}

void* package_serialize(t_package* package, int bytes) {
	void* magic = malloc(bytes);
	int offset = 0;

	memcpy(magic + offset, &(package->op_code), sizeof(int));
	offset += sizeof(int);
	memcpy(magic + offset, &(package->buffer->size), sizeof(int));
	offset += sizeof(int);
	memcpy(magic + offset, package->buffer->stream, package->buffer->size);
	offset += package->buffer->size;

	return magic;
}

void package_add(t_package* package, void* value, int size) {
	package->buffer->stream = realloc(package->buffer->stream, package->buffer->size + size + sizeof(int));

	memcpy(package->buffer->stream + package->buffer->size, &size, sizeof(int));
	memcpy(package->buffer->stream + package->buffer->size + sizeof(int), value, size);

	package->buffer->size += size + sizeof(int);
}

t_package* package_create(int cod_op) {
	t_package* package = malloc(sizeof(t_package));
	package->op_code = cod_op;
	package->buffer = malloc(sizeof(t_buffer));
	package->buffer->size = 0;
	package->buffer->stream = NULL;
	return package;
};

void package_delete(t_package* package) {
	free(package->buffer->stream);
	free(package->buffer);
	free(package);
}

int socket_initialize(char* ip, char* port) {
	struct addrinfo hints, *server_info;
	memset(&hints, 0, sizeof(hints));
	hints.ai_family = AF_UNSPEC;
	hints.ai_socktype = SOCK_STREAM;
	hints.ai_flags = AI_PASSIVE;
	getaddrinfo(ip, port, &hints, &server_info);

	// Creamos el socket de escucha del servidor
	int server_socket = socket(server_info->ai_family, server_info->ai_socktype, server_info->ai_protocol);

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

int socket_send_message(char* message, int target_socket) {
	t_package* package = malloc(sizeof(t_package));

	package->op_code = MENSAJE;
	package->buffer = malloc(sizeof(t_buffer));
	package->buffer->size = strlen(message) + 1;
	package->buffer->stream = malloc(package->buffer->size);
	memcpy(package->buffer->stream, message, package->buffer->size);

	return socket_send_package(package, target_socket);
}

int socket_send_package(t_package* package, int target_socket) {
	int bytes = package->buffer->size + 2 * sizeof(int);
	void* package_serialized = package_serialize(package, bytes);
	int send_ret = send(target_socket, package_serialized, bytes, MSG_NOSIGNAL);
	if (send_ret == -1) {
		socket_close(target_socket);
		printf("Cliente desconectado\n");
		// Migrar este mensaje a la función que lo llame, para que pueda salir por logger
	}
	free(package_serialized);
	package_delete(package);
	return send_ret;
}

void socket_close(int target_socket) {
	close(target_socket);
}

void* socket_receive_buffer(int* size, int target_socket) {
	void* buffer;
	recv(target_socket, size, sizeof(int), MSG_WAITALL);
	buffer = malloc(*size);
	recv(target_socket, buffer, *size, MSG_WAITALL);
	return buffer;
}

t_list* socket_receive_package(int target_socket) {
	int buffer_size;
	int offset = 0;
	t_list* params = list_create();
	int size;
	void* buffer = socket_receive_buffer(&buffer_size, target_socket);
	while (offset < buffer_size) {
		memcpy(&size, buffer + offset, sizeof(int));
		offset += sizeof(int);
		char* value = malloc(size + 1);	 // Add one byte for null terminator
		memcpy(value, buffer + offset, size);
		value[size] = '\0';	 // Add null terminator
		offset += size;
		list_add(params, value);
	}
	free(buffer);
	return params;
}

int socket_accept(int server_socket) {
	int target_socket = accept(server_socket, NULL, NULL);
	if (target_socket == -1) printf("Error de conexión al servidor\n");
	return target_socket;
}

int socket_receive_operation(int target_socket) {
	int cod_op;
	// Este if anidado acá parece que está llevando a varios bugs, como el doble Mensaje Recibido con info basura al principio
	// Agregar acá soporte para cierre de conexiones
	return recv(target_socket, &cod_op, sizeof(int), MSG_WAITALL) >= 0 ? cod_op : -1;
	// close(target_socket);
}

int socket_receive_message(int target_socket) {
	int size;
	char* buffer = socket_receive_buffer(&size, target_socket);
	printf("< %s\n", buffer);
	// Todo lo relacionado a OK_SEND_INSTRUCTIONS se va a ir cuando adapte
	// para que todas las instrucciones vayan en el mismo paquete
	if (strcmp(buffer, "OK_SEND_INSTRUCTIONS") == 0) {
		free(buffer);
		return 1;
	}
	free(buffer);
	return 0;
}

/*t_pcb* socket_receive_pcb(int target_socket){
	return serialize_receive_pcb(target_socket);
}*/

int connect_module(t_config* config, t_log* logger, char* module) {
	char* port = malloc(strlen("PUERTO_") + strlen(module) + 1);
	sprintf(port, "PUERTO_%s", module);
	char* ip = malloc(strlen("IP_") + strlen(module) + 1);
	sprintf(ip, "IP_%s", module);

	char* puerto_modulo = config_get_string_value(config, port);
	char* ip_modulo = config_get_string_value(config, ip);

	int module_socket = socket_initialize(ip_modulo, puerto_modulo);

	printf("Conectado a módulo %s\n", module);
	char* message = "prueba";
	socket_send_message(message, module_socket);

	return module_socket;
}

int receive_modules(t_log* logger, t_config* config) {
	// Obtenemos el port con el que escucharemos conexiones
	char* port = config_get_string_value(config, "PUERTO_ESCUCHA");

	log_info(logger, "El value del port es %s \n", port);

	// Inicializo el socket en el port cargado por la config
	int server_socket = socket_initialize_server(port);
	log_info(logger, "SOCKET INICIALIZADO");
	// Pongo el socket en modo de aceptar las escuchas
	// int cliente_fd = socket_accept(server_socket);

	return socket_accept(server_socket);
}

op_code return_opcode(char* code) {
	if (strcmp(code, "MENSAJE") == 0)
		return MENSAJE;
	else if (strcmp(code, "F_READ") == 0)
		return F_READ;
	else if (strcmp(code, "F_WRITE") == 0)
		return F_WRITE;
	else if (strcmp(code, "SET") == 0)
		return SET;
	else if (strcmp(code, "MOV_IN") == 0)
		return MOV_IN;
	else if (strcmp(code, "MOV_OUT") == 0)
		return MOV_OUT;
	else if (strcmp(code, "F_TRUNCATE") == 0)
		return F_TRUNCATE;
	else if (strcmp(code, "F_SEEK") == 0)
		return F_SEEK;
	else if (strcmp(code, "CREATE_SEGMENT") == 0)
		return CREATE_SEGMENT;
	else if (strcmp(code, "I/O") == 0)
		return I_O;
	else if (strcmp(code, "WAIT") == 0)
		return WAIT;
	else if (strcmp(code, "SIGNAL") == 0)
		return SIGNAL;
	else if (strcmp(code, "F_OPEN") == 0)
		return F_OPEN;
	else if (strcmp(code, "F_CLOSE") == 0)
		return F_CLOSE;
	else if (strcmp(code, "DELETE_SEGMENT") == 0)
		return DELETE_SEGMENT;
	else if (strcmp(code, "EXIT") == 0)
		return EXIT;
	else if (strcmp(code, "YIELD") == 0)
		return YIELD;
	else
		return -1;
}