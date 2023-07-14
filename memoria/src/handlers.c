#include "handlers.h"

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
		pthread_join(thread, NULL);
	}
}

// Manejo los receive con cada una de estas funciones
void handle_modules(t_memory_thread* mt) {
	/*if (!socket_send(socket_fs, serialize_message("OK_OPERATION", false))) {
		printf("Error al enviar el paquete\n");
		return -1;
	}*/
	log_warning(config_memory.logger, "Se conectó un módulo en el puerto %d", mt->socket);
	while (1) {
		t_package* package = socket_receive(mt->socket);
		t_instruction* instruction = deserialize_instruction(package);
		pthread_mutex_lock(&memory_access);
		switch ((t_memory_op)instruction->op_code) {
			// Se debe respetar el orden de los argumentos
			case MEM_READ_ADDRESS: {
				int s_id = atoi(list_get(instruction->args, 0));
				int offset = atoi(list_get(instruction->args, 1));
				int size = atoi(list_get(instruction->args, 2));
				int pid = atoi(list_get(instruction->args, 3));
				sleep(config_memory.mem_delay);
				char* buffer = read_memory(s_id, offset, size, mt->mem_structure, pid);
				if (buffer == NULL) // devolver seg_fault
					socket_send(mt->socket, package_new(SEG_FAULT));
				else // devolver buffer
					socket_send(mt->socket, serialize_message(buffer, false));
				break;
			}
			case MEM_WRITE_ADDRESS: {
				int s_id = atoi(list_get(instruction->args, 0));
				int offset = atoi(list_get(instruction->args, 1));
				char* buffer = list_get(instruction->args, 2);
				int size = strlen(buffer);
				int pid = atoi(list_get(instruction->args, 3));
				sleep(config_memory.mem_delay);
				if (write_memory(s_id, offset, size, buffer, mt->mem_structure, pid))
					socket_send(mt->socket, package_new(OK_INSTRUCTION));
				else
					socket_send(mt->socket, package_new(SEG_FAULT));
				break;
			}
			case MEM_INIT_PROCESS: {
				// Creo la tabla de segmentos y la devuelvo al kernel cuando crea un proceso
				int pid = atoi(list_get(instruction->args, 0));
				// t_list* segment_table = create_sg_table(mt->mem_structure, pid);
				log_info(config_memory.logger, "Creación de Proceso PID: %d", pid);
				// Envío la tabla de segmentos al kernel
				// socket_send(mt->socket, serialize_segment_table(segment_table));
				// Definir qué recibe Kernel de acá
				break;
			}
			case MEM_END_PROCCESS: {
				int pid = atoi(list_get(instruction->args, 0));
				remove_sg_table(mt->mem_structure, pid);
				log_info(config_memory.logger, "Eliminación de Proceso PID: %d", pid);
				break;
			}
			case MEM_CREATE_SEGMENT: {
				int s_id = atoi(list_get(instruction->args, 0));
				int size = atoi(list_get(instruction->args, 1));
				int pid = atoi(list_get(instruction->args, 2));
				int flag = add_segment(mt->mem_structure, pid, size, s_id);
				switch (flag) {
					case 1:
						// Devuelvo solicitud de compactación
						socket_send(mt->socket, package_new(COMPACT_ALL));
						break;
					case 2:
						// Devuelvo no hay espacio suficiente
						socket_send(mt->socket, package_new(NO_SPACE_LEFT));
						break;
					default:
						// Devuelvo la base del segmento creado
						socket_send(mt->socket, serialize_message((char*)&flag, false));
						break;
				}
				break;
			}
			case MEM_DELETE_SEGMENT: {
				int s_id = atoi(list_get(instruction->args, 0));
				int pid = atoi(list_get(instruction->args, 2));
				delete_segment(mt->mem_structure, pid, s_id);
				break;
			}
			case COMPACT_ALL: {
				log_info(config_memory.logger, "Solicitud de compactación");
				sleep(config_memory.com_delay);
				compact_memory(mt->mem_structure);
				break;
			}
			default: {
				log_error(config_memory.logger, "El proceso recibió algo no esperado, finalizando modulo");
				exit(1);
				break;
			}
		}
		pthread_mutex_unlock(&memory_access);
	}
}