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
	t_instruction* instruction_set = s_malloc(sizeof(t_instruction));
	instruction_set->op_code = SET;
	instruction_set->args = list_create();
	list_add(instruction_set->args, "AX");
	list_add(instruction_set->args, "1");

	t_instruction* instruction_yield = s_malloc(sizeof(t_instruction));
	instruction_yield->op_code = YIELD;
	instruction_yield->args = list_create();

	t_instruction* instruction_exit = s_malloc(sizeof(t_instruction));
	instruction_exit->op_code = EXIT;
	instruction_exit->args = list_create();

	t_queue* instructions = queue_create();
	queue_push(instructions, instruction_set);
	queue_push(instructions, instruction_yield);
	queue_push(instructions, instruction_exit);

	execution_context* context = s_malloc(sizeof(execution_context));
	context->instructions = instructions;
	context->program_counter = 0;
	context->updated_state = NEW;
	context->cpu_register = s_malloc(sizeof(cpu_register));  // inicializa el puntero
	context->segment_table = s_malloc(sizeof(segment_table));

	return context;
}