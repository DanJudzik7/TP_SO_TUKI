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
	// Inicializo la memoria (ESTA ES LA MEMORIA REAL, OSEA MI HEAP)
	void* memory = s_malloc(memory_shared.memory_size);

	// Inicializo las estructuras de memoria
	memory_structure* memory_structure = s_malloc(sizeof(memory_structure));
	memory_structure->hole_list = list_create();
	memory_structure->table_pid_segments = dictionary_create();
	memory_structure->segment_zero = s_malloc(sizeof(segment));
	// Recordemos que la ram es una t_list unicamente de ayuda, que apunta a las direcciones de memoria [dir_segment_zer, sig direccion, etc ]
	memory_structure->ram = list_create();
	
	// Creo el segmento 0 y lo agrego al diccionario y a la memoria auxiliar ram
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
	//No es necesario cargar el hole en la ram, 
	//Cargamos procesos y si eliminamos uno lo mandamos a hole_list pero sigue en la ram hasta que borremos y compactemos
	//list_add(memory_structure->ram,hole);

	log_info(memory_config.logger, "Memoria inicializada correctamente");
	printf("\nNuestro Heap de memoria ram arranca en: %p\n\n",  memory_structure->segment_zero->base);
	// Creo los hilos 
	//listen_modules(socket_memory,memory_structure);
	
 	//test_create_del_segm(memory_structure, memory);
	//test_compact(memory_structure, memory);
	test_rw(memory_structure, memory);

	log_destroy(memory_config.logger);
	config_destroy(memory_config.config);
	dictionary_clean_and_destroy_elements(memory_structure->table_pid_segments,free);
}


void segmento_hardcodeado(int PID, int SEGMENTO, memory_structure* memory_structure) {
    create_sg_table(memory_structure, PID);
    log_info(memory_config.logger, "\nSe creó la tabla de segmentos del PID %i", PID);
   	int test = add_segment(memory_structure, PID, 128, 1);
}

void test_create_del_segm(memory_structure* memory_structure, void* memory_base){
	segmento_hardcodeado(0,1,memory_structure);
	segmento_hardcodeado(1,1,memory_structure);
	segmento_hardcodeado(2,1,memory_structure);
	segmento_hardcodeado(3,1,memory_structure);
	
	//Grafica toda la tabla de segmentos de todos los procesos
	graph_table_pid_segments(memory_structure->table_pid_segments, memory_base);
	// Función para graficar la RAM
	graph_ram(memory_structure, memory_base);

	delete_segment(memory_structure,2,1);
	delete_segment(memory_structure,3,1);
	//Grafica toda la tabla de segmentos de todos los procesos
	graph_table_pid_segments(memory_structure->table_pid_segments, memory_base);
	// Función para graficar la RAM
	graph_ram(memory_structure, memory_base);
}


void test_compact(memory_structure* memory_structure, void* memory_base){
	segmento_hardcodeado(0,1,memory_structure);
	segmento_hardcodeado(1,1,memory_structure);
	segmento_hardcodeado(2,1,memory_structure);
	segmento_hardcodeado(3,1,memory_structure);
	delete_segment(memory_structure,1,1);
	// Función para graficar la RAM
	graph_ram(memory_structure, memory_base);

	compact_memory(memory_structure);

	// Función para graficar la RAM
	graph_ram(memory_structure, memory_base);
	graph_table_pid_segments(memory_structure->table_pid_segments, memory_structure->segment_zero->base);
}

void test_rw(memory_structure* memory_structure, void* memory_base){
	//segmento_hardcodeado(1,1,memory_structure);
	segmento_hardcodeado(1,1,memory_structure);
	log_info(memory_config.logger, "Iniciando escritura");
	if(write_memory(1,4,sizeof("hola"),"hola",memory_structure,1)){
		log_info(memory_config.logger, "Se escribió correctamente en memoria");
		char* buffer = read_memory(1,4,sizeof("hola"),memory_structure,1);
		log_info(memory_config.logger, "El valor leido es: %s", buffer);
	}else 
		log_info(memory_config.logger, "No se pudo escribir en memoria");
}