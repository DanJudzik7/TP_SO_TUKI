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

void print_execution_context(t_execution_context* execution_context) {
    printf("Execution Context:\n");
    printf(" - Program Counter: %u\n", execution_context->program_counter);
    
    t_registers* registers = execution_context->cpu_register;
    printf(" - CPU Registers:\n");
    printf("    - AX: %.*s\n", 4, registers->AX);
    printf("    - BX: %.*s\n", 4, registers->BX);
    printf("    - CX: %.*s\n", 4, registers->CX);
    printf("    - DX: %.*s\n", 4, registers->DX);
    printf("    - EAX: %.*s\n", 8, registers->EAX);
    printf("    - EBX: %.*s\n", 8, registers->EBX);
    printf("    - ECX: %.*s\n", 8, registers->ECX);
    printf("    - EDX: %.*s\n", 8, registers->EDX);
    printf("    - RAX: %.*s\n", 16, registers->RAX);
    printf("    - RBX: %.*s\n", 16, registers->RBX);
    printf("    - RCX: %.*s\n", 16, registers->RCX);
    printf("    - RDX: %.*s\n", 16, registers->RDX);
    
	/* segment_table* segment_table = execution_context->segment_table;
    printf(" - Segment Table - \n");
    printf("    - ID: %u\n", segment_table->pid);
    printf("    - Direction: %p\n", segment_table->segment_table_pcb);
	*/
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
void destroy_element(void* element) {
	free(element);
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