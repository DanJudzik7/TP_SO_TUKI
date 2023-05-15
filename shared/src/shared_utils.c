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

t_package* package_new_dict(int32_t key, void* value, uint64_t* value_size) {
	t_package* package = package_new(key);
	package_add(package, value, value_size);
	return package;
}

void package_nest(t_package* package, t_package* nested) {
	package_close(nested); // Convierte el package al tipo Serialized
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
}

t_package* package_new(int32_t type) {
	t_package* package = s_malloc(sizeof(t_package));
	package->type = type;
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
    memcpy(stream + offset, &(package->type), sizeof(int32_t));
    offset += sizeof(int32_t);
    memcpy(stream + offset, package->buffer, package->size);
    package->type = SERIALIZED;
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

t_package* serialize_cpu_registers(cpu_register* registers) {
	t_package* package = package_new(CPU_REGISTERS);
	uint64_t size4 = 4, size8 = 8, size16 = 16;
	// Registros de 4 bytes
	package_add(package, &(registers->register_4.AX), &size4);
	package_add(package, &(registers->register_4.BX), &size4);
	package_add(package, &(registers->register_4.CX), &size4);
	package_add(package, &(registers->register_4.DX), &size4);
	// Registros de 8 bytes
	package_add(package, &(registers->register_8.EAX), &size8);
	package_add(package, &(registers->register_8.EBX), &size8);
	package_add(package, &(registers->register_8.ECX), &size8);
	package_add(package, &(registers->register_8.EDX), &size8);
	// Registros de 16 bytes
	package_add(package, &(registers->register_16.RAX), &size16);
	package_add(package, &(registers->register_16.RBX), &size16);
	package_add(package, &(registers->register_16.RCX), &size16);
	package_add(package, &(registers->register_16.RDX), &size16);
	return package;
}

cpu_register* deserialize_cpu_registers(void* source) {
	uint64_t offset = 0;
	cpu_register* registers = s_malloc(sizeof(cpu_register));
	// Registros de 4 bytes
	package_decode_buffer(source, registers->register_4.AX, &offset);
	package_decode_buffer(source, registers->register_4.BX, &offset);
	package_decode_buffer(source, registers->register_4.CX, &offset);
	package_decode_buffer(source, registers->register_4.DX, &offset);
	// Registros de 8 bytes
	package_decode_buffer(source, registers->register_8.EAX, &offset);
	package_decode_buffer(source, registers->register_8.EBX, &offset);
	package_decode_buffer(source, registers->register_8.ECX, &offset);
	package_decode_buffer(source, registers->register_8.EDX, &offset);
	// Registros de 16 bytes
	package_decode_buffer(source, registers->register_16.RAX, &offset);
	package_decode_buffer(source, registers->register_16.RBX, &offset);
	package_decode_buffer(source, registers->register_16.RCX, &offset);
	package_decode_buffer(source, registers->register_16.RDX, &offset);
	return registers;
}

t_package* serialize_segment_table(segment_table* st) {
	t_package* package = package_new(SEGMENT_TABLE);
	// Implementar segment tables acá. Por ahora, manda solo un dato de ejemplo.
	uint64_t size = sizeof(uint32_t);
	package_add(package, &(st->id), &size);
	return package;
}

segment_table* deserialize_segment_table(void* source) {
	uint64_t offset = 0;
	segment_table* st = s_malloc(sizeof(segment_table));
	// Implementar segment tables acá. Por ahora, manda solo un dato de ejemplo.
	package_decode_buffer(source, &(st->id), &offset);
	return st;
}

t_package* serialize_execution_context(execution_context* ec) {
	t_package* package = package_new(EXECUTION_CONTEXT);
	uint64_t size4 = 4;
	package_nest(package, serialize_instructions(ec->instructions, true));
	package_nest(package, package_new_dict(PROGRAM_COUNTER, &(ec->program_counter), &size4));
	package_nest(package, package_new_dict(UPDATED_STATE, &(ec->updated_state), &size4));
	package_nest(package, serialize_cpu_registers(ec->cpu_register));
	package_nest(package, serialize_segment_table(ec->segment_table));
	return package;
}

execution_context* deserialize_execution_context(t_package* package) {
	execution_context* ec = s_malloc(sizeof(execution_context));
	uint64_t offset = 0;
	uint64_t offset_start = 0;
	while (package_decode_isset(package, offset)) {
		t_package* nested_package = package_decode(package->buffer, &offset);
		switch (nested_package->type) {
			case EC_INSTRUCTIONS:
				ec->instructions = queue_create();
				deserialize_instructions(nested_package, ec->instructions);
				break;
			case PROGRAM_COUNTER:
				package_decode_buffer(nested_package->buffer, &(ec->program_counter), &offset_start);
				break;
			case UPDATED_STATE:
				package_decode_buffer(nested_package->buffer, &(ec->updated_state), &offset_start);
				break;
			case CPU_REGISTERS:
				ec->cpu_register = deserialize_cpu_registers(nested_package->buffer);
				break;
			case SEGMENT_TABLE:
				ec->segment_table = deserialize_segment_table(nested_package->buffer);
				break;
			default:
				printf("Error: Tipo de paquete desconocido.\n");
				return NULL;
		}
		package_destroy(nested_package);
	}
	return ec;
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

t_package* serialize_instructions(t_queue* instructions, bool is_ec) {
	t_package* package = package_new(is_ec ? EC_INSTRUCTIONS : INSTRUCTIONS);
	// Recorre la cola de instrucciones y las serializa
	for (int i = 0; i < queue_size(instructions); i++) {
		t_instruction* instruction = list_get(instructions->elements, i);
		t_package* nested = package_new(instruction->op_code);
		// Serializa los args de la instrucción
		for (int j = 0; j < list_size(instruction->args); j++) package_write(nested, list_get(instruction->args, j));
		package_nest(package, nested);
	}
	return package;
}

t_package* parse_instruction(char* instruction) {
	char** parts = string_split(instruction, " ");
	// Creamos el package con el operation code correspondiente
	t_package* package = package_new(parse_op_code(parts[0]));
	for (int i = 1; parts[i] != NULL; i++) package_write(package, parts[i]);
	return package;
}

void deserialize_instructions(t_package* package, t_queue* instructions) {
	uint64_t offset = 0;
	// Va buscando todas las instrucciones
	while (package_decode_isset(package, offset)) {
		t_package* instruction_package = package_decode(package->buffer, &offset);
		t_instruction* instruction = s_malloc(sizeof(t_instruction));
		instruction->op_code = instruction_package->type;
		instruction->args = list_create();
		uint64_t offset_list = 0;
		// Carga los args de package_add
		while (package_decode_isset(instruction_package, offset_list)) list_add(instruction->args, package_decode_string(instruction_package->buffer, &offset_list));
		queue_push(instructions, instruction);
		package_destroy(instruction_package);
	}
}

bool package_decode_isset(t_package* package, uint64_t offset) {
	return offset < package->size;
}

char* package_decode_string(void* source, uint64_t* offset) {
	uint64_t *size = s_malloc(sizeof(uint64_t));
	memcpy(size, source + *offset, sizeof(uint64_t));
	*offset += sizeof(uint64_t);
	char *str = s_malloc(*size + 1);
	memcpy(str, source + *offset, *size);
	*offset += *size;
	free(size);
	return str;
}

void package_decode_buffer(void* source, void* dest, uint64_t* offset) {
	uint64_t *size = s_malloc(sizeof(uint64_t));
	memcpy(size, source + *offset, sizeof(uint64_t));
	*offset += sizeof(uint64_t);
	memcpy(dest, source + *offset, *size);
	*offset += *size;
	free(size);
}

t_package* package_decode(void* source, uint64_t* offset) {
	t_package* package = s_malloc(sizeof(t_package));
	memcpy(&(package->size), source + *offset, sizeof(uint64_t));
	*offset += sizeof(uint64_t);
	memcpy(&(package->type), source + *offset, sizeof(int32_t));
	*offset += sizeof(int32_t);
	package->buffer = s_malloc(package->size);
	memcpy(package->buffer, source + *offset, package->size);
	*offset += package->size;
	return package;
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
	/*if (!socket_send_message(module_socket, "Mensaje de prueba", false)) {
		printf("Error al enviar mensaje de prueba en %s en %d\n", module, module_socket);
		return -1;
	}*/
	printf("Conectado a módulo %s en socket %d\n", module, module_socket);
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

op_code parse_op_code(char* code) {
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