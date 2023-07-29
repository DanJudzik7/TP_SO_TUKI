#include "memoria.h"

extern t_config_memory config_memory;

int main(int argc, char** argv) {
	setup_config(argv[1] != NULL ? argv[1] : "first");
	log_info(config_memory.logger, "Iniciando Memoria...");
	int server_memory = socket_initialize_server(config_memory.port);  // Inicializo el socket en el puerto cargado por la config
	if (server_memory == -1) {
		log_error(config_memory.logger, "No se pudo inicializar el socket de servidor");
		exit(EXIT_FAILURE);
	}
	log_warning(config_memory.logger, "Socket de servidor inicializado en puerto %s", config_memory.port);

	// Inicializo la memoria
	void* memory = s_malloc(config_memory.memory_size);
	memset(memory, 0, config_memory.memory_size);
	t_memory_structure* memory_structure = new_memory_structure(memory);

	// Creo los hilos
	listen_modules(server_memory, memory_structure);

	log_warning(config_memory.logger, "Finalizando la Memoria");
	log_destroy(config_memory.logger);
	config_destroy(config_memory.config);
	dictionary_clean_and_destroy_elements(memory_structure->table_pid_segments, free);
	return 0;
}