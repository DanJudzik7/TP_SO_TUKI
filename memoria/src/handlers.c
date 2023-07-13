#include "handlers.h"
pthread_mutex_t mutex_compact;
pthread_mutex_t mutex_write;

// Manejo los receive con cada una de estas funciones
void handle_fs(thread* thread_fs) {
	/*printf("El código de operación es: %i\n", instruction->op_code);
	if (!socket_send(socket_fs, serialize_message("OK_OPERATION", false))) {
		printf("Error al enviar el paquete\n");
		return -1;
	}*/
	printf("Se conecto el File System en el puerto %d\n", thread_fs->socket);
	while (1) { 
	t_package* package_fs = socket_receive(thread_fs->socket);
	t_instruction* instruction = deserialize_instruction_test(package_fs);
		switch (instruction->op_code) {
			case F_WRITE:
				// El orden no es necesariamente este
				int s_id = atoi(list_get(instruction->args,0));
				int offset = atoi(list_get(instruction->args,1));
				int size = atoi(list_get(instruction->args,2));
				int pid = atoi(list_get(instruction->args,3));
				sleep(memory_shared.mem_delay);
				char* buffer = read_memory(s_id, offset, size, thread_fs->mem_structure, pid);
				if (buffer == NULL) {
					// devolver seg_fault
					socket_send(thread_fs->socket, package_new(SEG_FAULT));
				} else {
					// devolver buffer
					socket_send(thread_fs->socket, serialize_message(buffer, false));
				}
				break;
			case F_READ:
				// El orden no es necesariamente este
				int s_id_r = atoi(list_get(instruction->args,0));
				int offset_r = atoi(list_get(instruction->args,1));
				char* buffer_r = list_get(instruction->args,2);
				int size_r = strlen(buffer_r);
				int pid_r = atoi(list_get(instruction->args,3));
				sleep(memory_shared.mem_delay);
				//pthread_mutex_lock(&mutex_write);
				if (write_memory(s_id, offset, size,buffer,thread_fs->mem_structure,pid)) {
					socket_send(thread_fs->socket, package_new(OK_INSTRUCTION));
				} else {
					socket_send(thread_fs->socket, package_new(SEG_FAULT));
				}
				//pthread_mutex_unlock(&mutex_write);
				break;
			
			default:
				log_error(memory_config.logger, "El proceso recibió algo indebido, finalizando modulo");
				exit(1);
				break;
		}
	}
}

void handle_cpu(thread* thread_cpu) {
	while (1) {
		t_package* package_cpu = socket_receive(thread_cpu->socket);
		if (package_cpu == NULL) {
			printf("El cliente se desconectó\n");
			exit(1);
		}

		segment_read_write* segment_rw = deserialize_segment_read_write(package_cpu);

		switch (package_cpu->type) {
			case F_READ:
				sleep(memory_shared.mem_delay);
				char* buffer = read_memory(segment_rw->s_id, segment_rw->offset, segment_rw->size, thread_cpu->mem_structure, segment_rw->pid);
				if (buffer == NULL) {
					// devolver seg_fault
					socket_send(thread_cpu->socket, package_new(SEG_FAULT));
				} else {
					// devolver buffer
					socket_send(thread_cpu->socket, serialize_message(buffer, false));
				}
				break;
			case F_WRITE:
				sleep(memory_shared.mem_delay);
				pthread_mutex_lock(&mutex_write);
				if (write_memory(segment_rw->s_id, segment_rw->offset, segment_rw->size, segment_rw->buffer, thread_cpu->mem_structure, segment_rw->pid)) {
					// devolver ok
					socket_send(thread_cpu->socket, package_new(OK_INSTRUCTION));
				} else {
					// devolver seg_fault
					socket_send(thread_cpu->socket, package_new(SEG_FAULT));
				}
				pthread_mutex_unlock(&mutex_write);
				break;

			default:
				log_error(memory_config.logger, "El proceso recibió algo indebido, finalizando modulo");
				exit(1);
				break;
		}
	}
}

void handle_kernel(thread* thread) {
	while (1) {
		t_package* package_kernel = socket_receive(thread->socket);
		if (package_kernel == NULL) {
			printf("El cliente se desconectó\n");
			exit(1);
		}

		segment_table* sg = deserialize_segment_table(package_kernel);

		char* process_id = NULL;
		sprintf(process_id, "%d", sg->pid);

		switch (package_kernel->type) {
			case CREATE_PROCESS_MEMORY: {  // Creo la tabla de segmentos y la devuevlo al kernel cuando crea un proceso
				t_list* segment_table = create_sg_table(thread->mem_structure, sg->pid);
				log_info(memory_config.logger, "Creación de Proceso PID: %s", process_id);
				// Envio la tabla de segmentos al kernel
				//socket_send(socket_kernel, serialize_segment_table(segment_table));	 // TODO: ver serializacion
				break;
			}
			case END_PROCCESS_MEMORY: {
				remove_sg_table(thread->mem_structure, sg->pid);
				log_info(memory_config.logger, "Eliminación de Proceso PID: %s", process_id);
				break;
			}
			case CREATE_SEGMENT_MEMORY: {
				int flag = add_segment(thread->mem_structure, sg->pid, sg->size_segment, sg->s_id);
				switch (flag) {
					case 1:
						// Devuelvo solicitud de compactacion
						socket_send(thread->socket, package_new(COMPACT_MEMORY));
						break;
					case 2:
						// Devuelvo no hay espacio suficiente
						socket_send(thread->socket, package_new(NO_SPACE_LEFT));
						break;
					default:
						// Devuelvo la base del segmento creado
						socket_send(thread->socket, serialize_message((char*)&flag, false));
						break;
				}
				break;
			}
			case DELETE_SEGMENT_MEMORY:
				delete_segment(thread->mem_structure, sg->pid, sg->s_id);
				break;
			case COMPACT_MEMORY:
				log_info(memory_config.logger, "Solicitud de compactación");
				sleep(memory_shared.com_delay);
				pthread_mutex_lock(&mutex_compact);
				compact_memory(thread->mem_structure);
				pthread_mutex_unlock(&mutex_compact);
				break;

			default:
				log_error(memory_config.logger, "El proceso recibió algo indebido, finalizando modulo");
				exit(1);
				break;
		}
	}
}