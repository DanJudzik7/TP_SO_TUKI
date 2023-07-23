#include "handlers.h"

extern t_config_memory config_memory;
pthread_mutex_t memory_access;

void listen_modules(int server_memory, t_memory_structure* memory_structure) {
	pthread_mutex_init(&memory_access, NULL);
	while (1) {
		pthread_t thread;
		t_memory_thread* memory_thread = s_malloc(sizeof(t_memory_thread));
		memory_thread->socket = socket_accept(server_memory);
		if (memory_thread->socket == -1) {
			log_warning(config_memory.logger, "Hubo un error aceptando la conexión");
			continue;
		}
		memory_thread->mem_structure = memory_structure;
		pthread_create(&thread, NULL, (void*)handle_modules, memory_thread);
	}
}

// Manejo los receive con cada una de estas funciones
void handle_modules(t_memory_thread* mt) {
	log_warning(config_memory.logger, "Se conectó un módulo en el socket %d", mt->socket);
	while (1) {
		t_package* package = socket_receive(mt->socket);
		if (package == NULL) {
			log_error(config_memory.logger, "El módulo del socket %d desconectó", mt->socket);
			break;
		}
		t_instruction* instruction = deserialize_instruction(package);
		pthread_mutex_lock(&memory_access);
		switch ((t_memory_op)instruction->op_code) {
			// Se debe respetar el orden de los argumentos, si maxi, pero no es este el orden.
			// EL s_id es el 4 ponele y aca esta en el primero que recibo
			case MEM_READ_ADDRESS: {
				int s_id = atoi(list_get(instruction->args, 0));
				int offset = atoi(list_get(instruction->args, 1));
				int size = atoi(list_get(instruction->args, 2));
				int pid = atoi(list_get(instruction->args, 3));
				usleep(config_memory.access_delay);
				char* buffer = read_memory(s_id, offset, size, mt->mem_structure, pid);
				t_package* package = buffer == NULL ? package_new(SEG_FAULT) : serialize_message(buffer, false);
				if (!socket_send(mt->socket, package))
					log_error(config_memory.logger, "Error al enviar resultado al socket %d", mt->socket);
				break;
			}
			case MEM_WRITE_ADDRESS: {
				int s_id = atoi(list_get(instruction->args, 0));
				int offset = atoi(list_get(instruction->args, 1));
				char* buffer = list_get(instruction->args, 2);
				int size = strlen(buffer);
				int pid = atoi(list_get(instruction->args, 3));
				usleep(config_memory.access_delay);
				bool success = write_memory(s_id, offset, size, buffer, mt->mem_structure, pid);
				if (!socket_send(mt->socket, package_new(success ? OK_INSTRUCTION : SEG_FAULT)))
					log_error(config_memory.logger, "Error al enviar resultado al socket %d", mt->socket);
				break;
			}
			case MEM_INIT_PROCESS: {
				// Creo la tabla de segmentos y la devuelvo al kernel cuando crea un proceso
				int pid = atoi(list_get(instruction->args, 0));
				t_list* segment_table = create_sg_table(mt->mem_structure, pid);
				log_info(config_memory.logger, "Creación de Proceso PID: %d", pid);
				// Envío la tabla de segmentos al kernel
				if (!socket_send(mt->socket, serialize_segments_table(segment_table, SEGMENTS_TABLE, mt->mem_structure->heap)))
					log_error(config_memory.logger, "Error al enviar resultado al socket %d", mt->socket);
				// Definir qué recibe Kernel de acá
				break;
			}
			case MEM_END_PROCCESS: {
				int pid = atoi(list_get(instruction->args, 0));
				remove_sg_table(mt->mem_structure, pid);
				log_info(config_memory.logger, "Eliminación de Proceso PID: %d", pid);
				if (!socket_send(mt->socket, package_new(OK_INSTRUCTION)))
					log_error(config_memory.logger, "Error al enviar resultado al socket %d", mt->socket);
				break;
			}
			case MEM_CREATE_SEGMENT: {
				int s_id = atoi(list_get(instruction->args, 0));
				int size = atoi(list_get(instruction->args, 1));
				int pid = atoi(list_get(instruction->args, 2));
				int flag = add_segment(mt->mem_structure, pid, size, s_id);
				t_package* req_package;
				switch (flag) {
					case 1: // Devuelvo solicitud de compactación
						req_package = package_new(COMPACT_REQUEST);
						break;
					case 2: // Devuelvo no hay espacio suficiente
						req_package = package_new(OUT_OF_MEMORY);
						break;
					default: // Devuelvo la base del segmento creado
						req_package = serialize_message(string_itoa(flag), false);
						break;
				}
				if (!socket_send(mt->socket, req_package))
					log_error(config_memory.logger, "Error al enviar resultado al socket %d", mt->socket);
				break;
			}
			case MEM_DELETE_SEGMENT: {
				int s_id = atoi(list_get(instruction->args, 0));
				int pid = atoi(list_get(instruction->args, 1));
				delete_segment(mt->mem_structure, pid, s_id);
				if (!socket_send(mt->socket, package_new(OK_INSTRUCTION)))
					log_error(config_memory.logger, "Error al enviar resultado al socket %d", mt->socket);
				break;
			}
			case MEM_COMPACT_ALL: {
				log_info(config_memory.logger, "Solicitud de compactación");
				usleep(config_memory.compact_delay);
				compact_memory(mt->mem_structure);
				if (!socket_send(mt->socket, serialize_all_segments_tables(mt->mem_structure)))
					log_error(config_memory.logger, "Error al enviar resultado al socket %d", mt->socket);
				break;
			}
			default: {
				log_error(config_memory.logger, "Paquete inválido recibido, será ignorado");
				package_destroy(package);
				break;
			}
		}
		pthread_mutex_unlock(&memory_access);
	}
}