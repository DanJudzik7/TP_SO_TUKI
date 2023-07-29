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
char* get_full_path(char* path) {
	char* directorio = getcwd(NULL, 0);
	if (directorio == NULL) {
		printf("Error al obtener el directorio actual\n");
		abort();
	}
	char* ruta_file = string_from_format("%s/%s", directorio, path);
	free(directorio);
	return ruta_file;
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

void* s_malloc(size_t size) {
	void* ptr = malloc(size);
	if (ptr == NULL) {
		printf("Error: No hay más memoria disponible.\n");
		abort();
	}
	return ptr;
}

bool is_in_list(t_list* list, char* value) {
	for (int i = 0; i < list_size(list); i++) {
		if (strcmp(list_get(list, i), value) == 0) return true;
	}
	return false;
}

t_instruction* instruction_new(int32_t op_code) {
	t_instruction* instruction = s_malloc(sizeof(t_instruction));
	instruction->op_code = op_code;
	instruction->args = list_create();
	return instruction;
}

t_instruction* instruction_duplicate(t_instruction* source) {
	t_instruction* instruction = s_malloc(sizeof(t_instruction));
	instruction->op_code = source->op_code;
	instruction->args = list_duplicate(source->args);
	return instruction;
}

void instruction_destroy(t_instruction* instruction) {
	list_destroy(instruction->args);
	free(instruction);
	instruction = NULL;
}

t_execution_context* execution_context_new(int pid) {
	t_execution_context* ec = s_malloc(sizeof(t_execution_context));
	ec->instructions = queue_create();
	ec->program_counter = 0;
	ec->cpu_register = s_malloc(sizeof(t_registers));
	memset(ec->cpu_register, 0, sizeof(t_registers));
	ec->segments_table = list_create();
	ec->pid = pid;
	ec->kernel_request = NULL;
	return ec;
}

void execution_context_destroy(t_execution_context* ec) {
	queue_destroy_and_destroy_elements(ec->instructions, (void*)instruction_destroy);
	free(ec->cpu_register);
	ec->cpu_register = NULL;
	list_destroy_and_destroy_elements(ec->segments_table, (void*)free);
	ec->segments_table = NULL;
	if (ec->kernel_request != NULL) instruction_destroy(ec->kernel_request);
	free(ec);
	ec = NULL;
}

char* read_op_code(op_code code) {
	switch (code) {
		case F_READ:
			return "F_READ";
		case F_WRITE:
			return "F_WRITE";
		case SET:
			return "SET";
		case MOV_IN:
			return "MOV_IN";
		case MOV_OUT:
			return "MOV_OUT";
		case F_TRUNCATE:
			return "F_TRUNCATE";
		case F_SEEK:
			return "F_SEEK";
		case CREATE_SEGMENT:
			return "CREATE_SEGMENT";
		case I_O:
			return "I/O";
		case WAIT:
			return "WAIT";
		case SIGNAL:
			return "SIGNAL";
		case F_OPEN:
			return "F_OPEN";
		case F_CLOSE:
			return "F_CLOSE";
		case DELETE_SEGMENT:
			return "DELETE_SEGMENT";
		case EXIT:
			return "EXIT";
		case YIELD:
			return "YIELD";
		default:
			return "INVALID";
	}
}