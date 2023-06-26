#include "handler_instruction_memoria.h"


void create_sg_table(memory_structure* memory_structure,int process_id){
    t_list* segment_table = list_create();
    printf("--------Creando una tabla de segmentos para el proceso con PID: %d\n", process_id);
    list_add(segment_table, memory_structure->segment_zero);
    char pid_str[10];  // Almacena el ID del proceso como una cadena de caracteres
    snprintf(pid_str, sizeof(pid_str), "%d", process_id);
    printf("--------Agregando una tabla de segmentos al diccionario\n");
    dictionary_put(memory_structure->table_pid_segments, pid_str, segment_table);
    printf("--------Tabla de segmentos creada y agregada correctamente\n\n");
}

void remove_sg_table(memory_structure* memory_structure,int process_id){
    char pid_str[10];  // Almacena el ID del proceso como una cadena de caracteres
    sprintf(pid_str, "%d", process_id);
    dictionary_remove(memory_structure->table_pid_segments,process_id); 
}

void add_segment(memory_structure* memory_structure,int process_id, int size,int s_id){
   	
	segment* segment = malloc(sizeof(segment));
	
	if (memory_shared.remaining_memory > size)
    {
       // Recorro la ram para asignar el segmento segun el algoritmo
       if (strcmp(memory_shared.algorithm,"BEST") == 0){
          segment = best_fit(memory_structure,size,process_id,s_id);
       } else if (strcmp(memory_shared.algorithm,"FIRST") == 0){
          segment = first_fit(memory_structure,size,process_id, s_id);
       } else if (strcmp(memory_shared.algorithm,"WORST") == 0){
        	segment = worst_fit(memory_structure,size,process_id,s_id);
       } else{
           log_error(memory_config.logger,"No se reconoce el algoritmo de planificacion");
           exit(1);
       }
	   // Si el segmento es nulo pero tengo espacio, debo compactar
	   if(segment == NULL){
		// Avisar compactacion
		}
	else{
        log_error(memory_config.logger,"No hay espacio suficiente para crear el segmento");
        // Manejo el error devolviendo a kernel no hay espacio suficiente
    }
	}
}
void delete_segment(memory_structure* memory_structure, int process_id, int s_id_to_delete) {

	struc_helper_delete* struc_helper_delete;

    // Obtengo la lista de segmentos del proceso a eliminar
    t_list* segment_table = dictionary_get(memory_structure->table_pid_segments, process_id);
    // Obtengo el segmento que coincida con el que quiero eliminar en la tabla de su pid
	for( int i = 1; i <= list_size(segment_table); i++){
		segment* segment_pid = list_get(segment_table, i);
		if( segment_pid->s_id == s_id_to_delete){
			// Asigno para tener el segmento mas a mano{
			struc_helper_delete = segment_pid;
			list_add(memory_structure->hole_list, segment_pid);
			break;
		}
	}
	//Recorro la ram para eliminarla
	for( int i = 1; i <= list_size(memory_structure->ram); i++ ){
		segment* segment_ram = list_get(memory_structure->ram, i);
		//Si tiene la misma base que la struct de ayuda
		if (segment_ram->base == struc_helper_delete->segment->base)
		{	// ELimino justo esa posicion
			list_remove(memory_structure->ram, i);
		}
	}
	//caso de que tenga huecos libres aledaños, los deberá consolidar actualizando sus estructuras administrativas.
	compact_hole_list(memory_structure);
}

void compact_hole_list(memory_structure* memory_structure){
	if(list_size(memory_structure->hole_list) != 1)
	{
		for (int i = 0; i < list_size(memory_structure->hole_list) ; i++)
		{
		// Mientras segment no sea el ultimo
		if (i < list_size(memory_structure->hole_list) - 1) {
		 	segment* segment_current = list_get(memory_structure->hole_list, i);
		 	// agarro su siguiente segmento para comparar
		 	segment* segment_next = list_get(memory_structure->hole_list, i +1 );
		 	// Si el sig segmento comienza dps del anterior los compacto y elimino el sigiente
		 	if(segment_current->base + segment_current->offset + 1 == segment_next->base){
					segment_current->offset += segment_next->offset;
					list_remove(memory_structure->hole_list, i +1);
					//Como compacte la memoria eliminando uno, el size se acorta por lo tanto debo avanzar 1 mas
					i++;
		 	}
		}
		}
	}
}

void swap(segment* a, segment* b) {
    segment temp = *a;
    *a = *b;
    *b = temp;
}

void more_close_to_heap(segment* a, segment* b){
	return a->base < b->base;
}

void compact_memory(memory_structure* memory_structure){

	if(list_size(memory_structure->hole_list > 1 )){ 
	// Ordeno la lista de hole para tener siempre a mano el mas cercano al heap
	list_sort(memory_structure->hole_list, more_close_to_heap);
		// Recorro la memoria ram auxiliar donde estan mis procesos actuales
		for (int i = 1; i < list_size(memory_structure->ram) ; i++){
			// Mientras que no este en la ultima pos de la ram auxiliar
			if(i < list_size(memory_structure->ram) - 1){
				//Agarro la posicion de memoria en la ram
				segment* ram_segment = list_get(memory_structure->ram, i);
				// agarro el primer hole que siempre va a ser el mas cercano al heap
				segment* hole_segment = list_get(memory_structure->hole_list,0);
				// Si el segmento en ram esta mas abajo que el hueco (osea tengo un hueco arriba en ram), hago el swap 
				if (ram_segment->base > hole_segment->base) {
       	     		swap(ram_segment, hole_segment);
       	    	}
			}
		list_sort(memory_structure->hole_list, more_close_to_heap);
		}
	}
}

char* read_memory(int s_id,int offset, int size,memory_structure* structures,int pid){
	char* buffer;
	segment* segment = get_segment_by_id(s_id,structures,pid);
	if (segment != NULL){
		if(segment-> base + offset + size > segment->base + segment->offset){
			log_error(memory_config.logger,"Segmentation fault, no se puede leer mas alla del segment");
			return NULL;
		}else {
			memcpy(&buffer, segment->base + offset, sizeof(char*));
			return buffer;
		}
	} else {
		log_error(memory_config.logger,"No se encontro el segmento solicitado");
		return NULL;
	}
}

bool write_memory(int s_id,int offset,int size,char* buffer,memory_structure* structures,int pid){
	segment* segment = get_segment_by_id(s_id,structures,pid);
	if (segment != NULL) {
		if(segment-> base + offset + size > segment->base + segment->offset){
			log_error(memory_config.logger,"Segmentation fault, no se puede escribir mas alla del segment");
			return false;
		}else {
			memcpy(segment->base + offset, buffer, sizeof(char*));
			return true;
		}
	} else {
		log_error(memory_config.logger,"No se encontro el segmento solicitado");
		return false;
	}
}

segment* get_segment_by_id(int s_id,memory_structure* structures,int pid){
	
	char pid_str[10];  // Almacena el ID del proceso como una cadena de caracteres
    sprintf(pid_str, "%d", pid);

	t_list* segment_table = dictionary_get(structures->table_pid_segments, pid);
	
	segment* segment;
	for (int i = 0; i < list_size(segment_table); i++) {
		segment = list_get(segment_table, i);
		if (segment->s_id == s_id) {
			return segment;
		}
	}
	return NULL;
}

/*
int instruction_handler_memoria(int socket_client) {
	// Todo esto no está adaptado al nuevo packaging

	while (1) {
		t_package* package = socket_receive(socket_client);
		if (package == NULL) {
			printf("El cliente se desconectó\n");
			break;
		}
		printf("El código de operación es: %i\n", package->type);
		// To do: Mejorar lo que hace acá. Tiene que recibir un paquete de tipo INSTRUCTION, con el t_instruction nesteado adentro

		switch (package->type) {
			case CREATE_SEGMENT:
				printf("RECIBIMOS UNA INSTRUCCIÓN DE CREAR SEGMENTO DE MEMORIA\n");
				bool success_creation_segment = create_memory_segment();
				if (!socket_send(socket_client, serialize_message(success_creation_segment ? "Eliminado": "Error", false))) {
					printf("Error al enviar el paquete\n");
					return -1;
				}
				break;
			case DELETE_SEGMENT:
				printf("RECIBIMOS UNA INSTRUCCIÓN DE ELIMINAR UN SEGMENTO DE MEMORIA\n");
				bool success_delete_segment = delete_memory_segment(0);  // ACTUALMENTE PASO UN ID = 0, PERO LUEGO NECESITO SABERLO DADO EL
				if (!socket_send(socket_client, serialize_message(success_delete_segment ? "Eliminado": "Error", false))) {
					printf("Error al enviar el paquete\n");
					return -1;
				}
				break;
			case MESSAGE_OK:
				printf("RECIBIMOS UN HANDSAKE\n");
				break;
			default:
				printf("Error al recibir código de operación\n");
				return -1;
		}
	}
	return 0;
}*/