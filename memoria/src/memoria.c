#include "memoria.h"

configuration_memory config_memory;
memory memory_shared;

int main(int argc, char** argv) {
	config_memory.logger = start_logger("memoria");
	config_memory.config = start_config("memoria");

	config_memory.algorithm = config_get_string_value(config_memory.config, "ALGORITMO_PLANIFICACION");
	memory_shared.segment_zero = config_get_string_value(config_memory.config, "TAM_SEGMENTO_0");
	memory_shared.size = config_get_string_value(config_memory.config, "TAM_MEMORIA");
	log_warning(config_memory.logger, "Iniciando la memoria");

	char* port = config_get_string_value(config_memory.config, "PUERTO_ESCUCHA");
	int socket_memory = socket_initialize_server(port);	 // Inicializo el socket en el puerto cargado por la config
	if (socket_memory == -1) {
		log_error(config_memory.logger, "No se pudo inicializar el socket de servidor");
		exit(EXIT_FAILURE);
	}
	log_warning(config_memory.logger, "Socket de servidor inicializado en puerto %s", port);

	// int cliente_fd = receive_modules(config_memory.logger,config_memory.config);
	// int conexion_cpu = connect_module(config_memory.config,config_memory.logger,"CPU");

	int cpu_socket = socket_accept(socket_memory);
	execution_context* context = create_context_test();

	socket_send(cpu_socket, serialize_execution_context(context));
	log_warning(config_memory.logger, "Context enviado a CPU");
	// instruction_handler_memoria(cliente_fd);
}

execution_context* create_context_test() {
	t_queue* instructions;

	t_list* sublist1 = list_create();
	list_add(sublist1, SET);
	list_add(sublist1, "AX");
	list_add(sublist1, "HOLA");

	t_list* sublist2 = list_create();
	list_add(sublist2, YIELD);

	t_list* sublist3 = list_create();
	list_add(sublist3, EXIT);

	instructions = list_create();
	list_add(instructions, sublist1);
	list_add(instructions, sublist2);
	list_add(instructions, sublist3);

	execution_context* context = s_malloc(sizeof(execution_context));
	context->instructions = instructions;
	context->program_counter = 0;
	context->updated_state = NEW;
	context->cpu_register = s_malloc(sizeof(cpu_register));  // inicializa el puntero
	context->segment_table = s_malloc(sizeof(segment_table));

	return context;
}