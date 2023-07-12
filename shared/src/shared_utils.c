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

void* s_malloc(size_t size) {
	void* ptr = malloc(size);
	if (ptr == NULL) {
		printf("Error: No hay más memoria disponible.\n");
		abort();
	}
	return ptr;
}

void print_execution_context(execution_context* execution_context) {
    printf("Execution Context:\n");
    printf(" - Program Counter: %u\n", execution_context->program_counter);
    printf(" - Updated State: %d\n", execution_context->updated_state);
    
    cpu_register* registers = execution_context->cpu_register;
    printf(" - CPU Registers:\n");
    printf("    - AX: %.*s\n", 4, registers->register_4.AX);
    printf("    - BX: %.*s\n", 4, registers->register_4.BX);
    printf("    - CX: %.*s\n", 4, registers->register_4.CX);
    printf("    - DX: %.*s\n", 4, registers->register_4.DX);
    printf("    - EAX: %.*s\n", 8, registers->register_8.EAX);
    printf("    - EBX: %.*s\n", 8, registers->register_8.EBX);
    printf("    - ECX: %.*s\n", 8, registers->register_8.ECX);
    printf("    - EDX: %.*s\n", 8, registers->register_8.EDX);
    printf("    - RAX: %.*s\n", 16, registers->register_16.RAX);
    printf("    - RBX: %.*s\n", 16, registers->register_16.RBX);
    printf("    - RCX: %.*s\n", 16, registers->register_16.RCX);
    printf("    - RDX: %.*s\n", 16, registers->register_16.RDX);
    
   /* segment_table* segment_table = execution_context->segment_table;
    printf(" - Segment Table - \n");
    printf("    - ID: %u\n", segment_table->pid);
    printf("    - Direction: %p\n", segment_table->segment_table_pcb);
	*/
}
