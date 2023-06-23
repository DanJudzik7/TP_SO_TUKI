#include "handler_instruction_memoria.h"


t_list* create_sg_table(memory_structure* memory_structure,int process_id){
    t_list* segment_table = list_create();
    printf("--------Creando una tabla de segmentos para el proceso con PID: %d\n", process_id);
    list_add(segment_table, memory_structure->segment_zero);
	printf("--------Agregando correctamente el segmento zero a la tabla\n");
    char pid_str[10];  // Almacena el ID del proceso como una cadena de caracteres
    snprintf(pid_str, sizeof(pid_str), "%d", process_id);
    printf("--------Agregando una tabla de segmentos al diccionario con PID: %s\n", pid_str);
    dictionary_put(memory_structure->table_pid_segments, pid_str, segment_table);
    printf("--------Tabla de segmentos creada y agregada correctamente\n\n");
    return segment_table;
}

void remove_sg_table(memory_structure* memory_structure,int process_id){
    char pid_str[10];  // Almacena el ID del proceso como una cadena de caracteres
    sprintf(pid_str, "%d", process_id);
    dictionary_remove(memory_structure->table_pid_segments,process_id); 
}

void add_segment(memory_structure* memory_structure,int process_id, int size,int s_id){
    if (memory_shared.remaining_memory > size)
    {
       // Recorro la ram para asignar el segmento segun el algoritmo
       if (strcmp(memory_shared.algorithm,"BEST") == 0){
           best_fit(memory_structure,size,process_id,s_id);
       } else if (strcmp(memory_shared.algorithm,"FIRST") == 0){
            first_fit(memory_structure,size,process_id, s_id);
       } else if (strcmp(memory_shared.algorithm,"WORST") == 0){
            worst_fit(memory_structure,size,process_id,s_id);
       } else{
           log_error(memory_config.logger,"No se reconoce el algoritmo de planificacion");
           exit(1);
       }
    } else{
        log_error(memory_config.logger,"No hay espacio suficiente para crear el segmento");
        // Manejo el error devolviendo a kernel no hay espacio suficiente
    }
    
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