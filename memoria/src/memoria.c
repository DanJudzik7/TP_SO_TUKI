#include "memoria.h"

// Debería tener una lista de listas, donde cada sub-lista es la tabla de segmentos de cada proceso.

int main() {
	setup_config();
	log_info(config_memory.logger, "Iniciando Memoria...");
	int server_memory = socket_initialize_server(config_memory.port);  // Inicializo el socket en el puerto cargado por la config
	if (server_memory == -1) {
		log_error(config_memory.logger, "No se pudo inicializar el socket de servidor");
		exit(EXIT_FAILURE);
	}
	log_warning(config_memory.logger, "Socket de servidor inicializado en puerto %s", config_memory.port);

	// Inicializo la memoria (ESTA ES LA MEMORIA REAL, O SEA MI HEAP)
	void* memory = s_malloc(config_memory.memory_size);
	log_info(config_memory.logger, "Memoria inicializada correctamente");
	t_memory_structure* memory_structure = new_memory_structure(memory);

	log_info(config_memory.logger, "Nuestro Heap de memoria ram arranca en: %p", memory_structure->segment_zero->base);
	// Creo los hilos
	listen_modules(server_memory, memory_structure);

	log_destroy(config_memory.logger);
	config_destroy(config_memory.config);
	dictionary_clean_and_destroy_elements(memory_structure->table_pid_segments, free);
	return 0;
}