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

	t_list* segments_list = list_create();
	t_list* hole_list = list_create();

	hole* hole = malloc(sizeof(hole));
	hole->base = memory + memory_shared.sg_zero_size;
	hole->size = memory_shared.memory_size - memory_shared.sg_zero_size;
	list_add(hole_list,hole);
	//addSegmentZero(memory,segments_list);  // Tecnicamente el segmento zero deberia ir al principio de cada proceso asi que ta ma

	listen_modules(socket_memory);
}

void addSegmentZero(void* memory,t_list* segments_list){
	segment* segmentZero;
	segmentZero->base = memory;
	segmentZero->offset = memory_shared.sg_zero_size;
	segmentZero->is_in_use = 1;
	segmentZero->pid = 0;

	list_add(segments_list,segmentZero);


}