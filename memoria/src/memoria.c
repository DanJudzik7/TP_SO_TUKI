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

	// Inicializo la memoria
	void* memory = s_malloc(memory_shared.memory_size);

	// Inicializo las estructuras de memoria
	memory_structure* memory_structure = s_malloc(sizeof(memory_structure));
	memory_structure->hole_list = list_create();
	memory_structure->table_pid_segments = dictionary_create();
	memory_structure->segment_zero = s_malloc(sizeof(segment));
	memory_structure->ram = list_create();
	
	// Creo el segmento 0 y lo agrego al diccionario y a la memoria
	memory_structure->segment_zero->base = memory;
	memory_structure->segment_zero->offset = memory_shared.sg_zero_size;
	memory_structure->segment_zero->s_id = 0;	
	dictionary_put(memory_structure->table_pid_segments ,(char *)&memory_structure->segment_zero->s_id, memory_structure->segment_zero);
	list_add(memory_structure->ram,memory_structure->segment_zero);
	memory_shared.remaining_memory -= memory_shared.sg_zero_size;
	
	// Creo el agujero inicial
	hole* hole = malloc(sizeof(hole));
	hole->base = memory + memory_shared.sg_zero_size;
	hole->size = memory_shared.memory_size - memory_shared.sg_zero_size;
	list_add(memory_structure->hole_list,hole);
	list_add(memory_structure->ram,hole);


	log_info(memory_config.logger, "Memoria inicializada correctamente");
	// Creo los hilos TODO: implementar hilos
	//listen_modules(socket_memory,memory_structure);
	segmento_hardcodeado(0,1,memory_structure);
	//Hasta aca termina perfecto

	//Grafica toda la tabla de segmentos de todos los procesos
	graph_table_pid_segments(memory_structure->table_pid_segments);
	// Función para graficar la RAM
	graph_ram(memory_structure->ram);

	log_destroy(memory_config.logger);
	config_destroy(memory_config.config);
	dictionary_clean_and_destroy_elements(memory_structure->table_pid_segments,free);
}

/*
void createSGZero(void* memory, segment* segmentZero){
	segmentZero->base = memory;
	segmentZero->offset = memory_shared.sg_zero_size;
	segmentZero->s_id = 0;	
}*/
void segmento_hardcodeado(int PID, int SEGMENTO, memory_structure* memory_structure) {

    t_list* tabla_de_segmentos_del_pid = create_sg_table(memory_structure, PID);
    log_info(memory_config.logger, "\nSe creó la tabla de segmentos del PID %d", PID);
	printf("----Su Dirección de memoria -> %p\n", (void*)tabla_de_segmentos_del_pid);
    add_segment(memory_structure, PID, 128, 1);
	// ESTA NO ES LAS DIRECCIONES REALES
	graph_specific_table_pid_segments(tabla_de_segmentos_del_pid, PID);

}
