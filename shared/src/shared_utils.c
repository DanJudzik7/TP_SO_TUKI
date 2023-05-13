#include "shared_utils.h"

t_config* start_config(char* module) {
	char* path_config = get_config_type(module, "config");
	t_config* config = config_create(path_config);
	if (config == NULL) {
		printf("Error al cargar la configuración\n");
		abort();
	}
	free(path_config);
	return config;
}

t_log* start_logger(char* module) {
	t_log* logger;
	char* path_log = get_config_type(module, "log");
	if ((logger = log_create(path_log, module, true, LOG_LEVEL_INFO)) == NULL) {
		printf("Error al crear logger\n");
		abort();
	}
	free(path_log);
	return logger;
}

char* get_config_type(char* module, char* file_type) {
	char* directorio = getcwd(NULL, 0);
	if (directorio == NULL) {
		printf("Error al obtener el directorio actual\n");
		abort();
	}
	char* ruta_config = string_from_format("%s/cfg/%s.%s", directorio, module, file_type);
	free(directorio);
	return ruta_config;
}

t_package* package_new_dict(int32_t key, void* value, uint64_t value_size) {
	t_package* package = package_new(key);
	package_add(package, value, &value_size);
	return package;
}

void package_nest(t_package* package, t_package* nested) {
	package_close(nested);
	uint64_t offset = package->size;
	package->size += nested->size;
	package->buffer = realloc(package->buffer, package->size);
	memcpy(package->buffer + offset, nested->buffer, nested->size);
	package_destroy(nested);
}

void package_write(t_package* package, char* value) {
	uint64_t size = strlen(value) + 1;
	package_add(package, value, &size);
}

void package_add(t_package* package, void* value, uint64_t* value_size) {
	uint64_t offset = package->size;
	package->size += sizeof(uint64_t) + *value_size;
	package->buffer = realloc(package->buffer, package->size);
	memcpy(package->buffer + offset, value_size, sizeof(uint64_t));
	memcpy(package->buffer + offset + sizeof(uint64_t), value, *value_size);
    if (*value_size == strlen((char*)value)) memcpy(package->buffer + offset + sizeof(uint64_t) + *value_size, "\0", 1);
}

t_package* package_new(int32_t field) {
	t_package* package = s_malloc(sizeof(t_package));
	package->field = field;
	package->size = 0;
	package->buffer = NULL;
	return package;
};

void package_close(t_package* package) {
    uint64_t package_size = sizeof(uint64_t) + sizeof(int32_t) + package->size;
    void* stream = s_malloc(package_size);
    uint64_t offset = 0;
    memcpy(stream + offset, &(package->size), sizeof(uint64_t));
    offset += sizeof(uint64_t);
    memcpy(stream + offset, &(package->field), sizeof(int32_t));
    offset += sizeof(int32_t);
    memcpy(stream + offset, package->buffer, package->size);
    package->field = SERIALIZED;
    package->size = package_size;
    free(package->buffer);
    package->buffer = stream;
}

void package_destroy(t_package* package) {
	free(package->buffer);
	package->buffer = NULL;
	free(package);
	package = NULL;
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

bool socket_send_message(int target_socket, char* message, bool is_flaw) {
	t_package* package = package_new(is_flaw ? MESSAGE_FLAW : MESSAGE_OK);
	package_write(package, message);
	return socket_send(target_socket, package);
}

bool socket_send(int target_socket, t_package* package) {
	package_close(package);
	bool send_ret = send(target_socket, package->buffer, package->size, MSG_NOSIGNAL) != -1;
	package_destroy(package);
	return send_ret;
}

void socket_close(int target_socket) {
	close(target_socket);
}

t_package* serialize_execution_context(execution_context* ec) { // En progreso
	// No llegué a mergear bien ni completar todo esto, así que hay un poco de todo. Si pueden déjenmelo para seguir a mí.
	// Serializar los campos individuales del execution_context y agregarlos al paquete
	t_package* package = package_new(EXECUTION_CONTEXT);
	// To do: Instructions
	/*package_nest(package, package_new_dict(PROGRAM_COUNTER, &(ec->program_counter), sizeof(uint32_t)));
	package_nest(package, package_new_dict(UPDATED_STATE, &(ec->updated_state), sizeof(int32_t)));*/
	// To do: CPU Registers, Segment Table
	// Esto reemplaza package_add_context por package_nest y package_new_dict, que permiten representar tipos mucho más amplios
	return package;
}

execution_context* deserialize_execution_context(t_package* package) { // En progreso
	// No llegué a mergear bien ni completar todo esto, así que hay un poco de todo. Si pueden déjenmelo para seguir a mí.
	execution_context* ec = s_malloc(sizeof(execution_context));
	//int offset = 0;

	/* memcpy(&ec->program_counter, package->buffer + offset, sizeof(uint32_t));
	offset += sizeof(uint32_t);
	memcpy(&ec->updated_state, package->buffer + offset, sizeof(process_state));
	offset += sizeof(process_state); */

	// Deserializar los campos individuales del paquete y asignarlos al execution_context
	/* t_list* params = list_create();
	memcpy(&(ec->program_counter), list_get(params, offset), sizeof(int));
	offset++;
	memcpy(&(ec->updated_state), list_get(params, offset), sizeof(process_state));
	offset++;
	ec->cpu_register = list_get(params, offset);
	offset++;
	ec->segment_table = list_get(params, offset);
	list_destroy_and_destroy_elements(params, free); */
	// Liberar la memoria utilizada por la lista y el paquete recibido
	return ec;
}

void* socket_receive_buffer(int target_socket, uint64_t size) {
	void* buffer = s_malloc(size);
	return recv(target_socket, buffer, (size_t)size, MSG_WAITALL) > 0 ? buffer : NULL;
}

uint64_t* socket_receive_long(int target_socket) {
	uint64_t* value = malloc(sizeof(uint64_t));
	return recv(target_socket, value, sizeof(uint64_t), MSG_WAITALL) > 0 ? value : NULL;
}

int32_t* socket_receive_int(int target_socket) {
	int32_t* value = malloc(sizeof(int32_t));
	return recv(target_socket, value, sizeof(int32_t), MSG_WAITALL) > 0 ? value : NULL;
}

t_package* socket_receive(int target_socket) {
	t_package* package = malloc(sizeof(t_package));
	uint64_t* size = socket_receive_long(target_socket);
	if (size == NULL) return NULL;
	package->size = *size;
	free(size);
	int32_t* field = socket_receive_int(target_socket);
	if (field == NULL) return NULL;
	package->field = *field;
	free(field);
	package->buffer = socket_receive_buffer(target_socket, package->size);
	if (package->buffer == NULL) return NULL;
	return package;
}

char* deserialize_message(t_package* package) {
	uint64_t *size = s_malloc(sizeof(uint64_t));
	memcpy(size, package->buffer, sizeof(uint64_t));
	char* message = s_malloc(*size + 1);
	memcpy(message, package->buffer + sizeof(uint64_t), *size);
	message[*size] = '\0';
	package_destroy(package);
	free(size);
	return message;
}

void deserialize_instructions(t_package* package, t_queue* instructions) {
	uint64_t offset = 0, item_offset = 0;
	uint64_t *size = s_malloc(sizeof(uint64_t));
	*size = 0;
	uint64_t *item_size = s_malloc(sizeof(uint64_t));
	*item_size = 0;
	// Va buscando todas las instrucciones
	while (offset < package->size) {
		t_instruction* instruction = s_malloc(sizeof(t_instruction));
		instruction->args = list_create();
		memcpy(size, package->buffer + offset, sizeof(uint64_t));
		offset += sizeof(uint64_t);
		memcpy(&instruction->op_code, package->buffer + offset, sizeof(int32_t));
		offset += sizeof(int32_t);
		char *args = s_malloc(*size + 1);
		memcpy(args, package->buffer + offset, *size);
		item_offset = offset;
		// Carga los args de package_add
		while ((offset - item_offset) < *size) {
			memcpy(item_size, package->buffer + offset, sizeof(uint64_t));
			offset += sizeof(uint64_t);
			char *item = s_malloc(*item_size + 1);
			memcpy(item, package->buffer + offset, *item_size);
			offset += *item_size;
			list_add(instruction->args, item);
		}
		queue_push(instructions, instruction);
		free(args);
	}
	free(item_size);
	free(size);
	package_destroy(package);
}

int socket_accept(int server_socket) {
	int target_socket = accept(server_socket, NULL, NULL);
	if (target_socket == -1) printf("Error de conexión al servidor\n");
	return target_socket;
}

void* s_malloc(size_t size) {
	void* ptr = malloc(size);
	if (ptr == NULL) {
		printf("Error: No hay más memoria disponible.\n");
		abort();
	}
	return ptr;
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
	if (!socket_send_message(module_socket, "Mensaje de prueba", false)) {
		// Mejorar error handling acá
		printf("Error al enviar mensaje de prueba\n");
		return -1;
	}
	printf("Conectado a módulo %s en %d\n", module, module_socket);
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

op_code get_opcode(char* code) {
	if (strcmp(code, "F_READ") == 0)
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