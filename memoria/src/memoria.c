#include "memoria.h"
// Deberia tener una lista de listas, donde cada sub-lista es la tabla de segmentos de cada proceso.

void main() {
	initialize();
	log_info(memory_config.logger, "Iniciando Memoria...");
	int socket_memory = socket_initialize_server(memory_config.port);	 // Inicializo el socket en el puerto cargado por la config
	if (socket_memory == -1) {
		log_error(memory_config.logger, "No se pudo inicializar el socket de servidor");
		exit(EXIT_FAILURE);
	}
	log_warning(memory_config.logger, "Socket de servidor inicializado en puerto %s", memory_config.port);

	void* memory = s_malloc(memory_shared.memory_size);
	structures structures;
	structures.hole_list = list_create();
	structures.all_segments = dictionary_create();
	structures.segment_zero = malloc(sizeof(segment));
	
	hole* hole = malloc(sizeof(hole));
	hole->base = memory + memory_shared.sg_zero_size;
	hole->size = memory_shared.memory_size - memory_shared.sg_zero_size;
	list_add(structures.hole_list,hole);
	
	createSGZero(memory,structures.segment_zero);
	dictionary_put(structures.all_segments , "id0", structures.segment_zero);
	free(structures.segment_zero); //no se si esta bien
	listen_modules(socket_memory,structures);
}

void createSGZero(void* memory, segment* segmentZero){
	segmentZero->base = memory;
	segmentZero->offset = memory_shared.sg_zero_size;
	segmentZero->s_id = 0;	
}