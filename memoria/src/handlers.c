#include "handlers.h"
pthread_mutex_t mutex_compact;
pthread_mutex_t mutex_write;

// Manejo los receive con cada una de estas funciones
void handle_fs(int socket_fs, memory_structure* memory_structure) {
	while (1) {
		t_package* package_fs = socket_receive(socket_fs);
		if (package_fs == NULL) {
			printf("El cliente se desconectó\n");
			exit(1);
			break;
		}

		segment_read_write* segment_rw = deserialize_segment_read_write(package_fs);

		switch (package_fs->type) {
			case F_READ:
				sleep(memory_shared.mem_delay);
				char* buffer = read_memory(segment_rw->s_id, segment_rw->offset, segment_rw->size, memory_structure, segment_rw->pid);
				if (buffer == NULL) {
					// devolver seg_fault
					socket_send(socket_fs, package_new(SEG_FAULT));
				} else {
					// devolver buffer
					socket_send(socket_fs, serialize_message(buffer, false));
				}
				break;
			case F_WRITE:
				sleep(memory_shared.mem_delay);
				pthread_mutex_lock(&mutex_write);
				if (write_memory(segment_rw->s_id, segment_rw->offset, segment_rw->size, segment_rw->buffer, memory_structure, segment_rw->pid)) {
					socket_send(socket_fs, package_new(OK_INSTRUCTION));
				} else {
					socket_send(socket_fs, package_new(SEG_FAULT));
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

void handle_cpu(int socket_cpu, memory_structure* memory_structure) {
	while (1) {
		t_package* package_cpu = socket_receive(socket_cpu);
		if (package_cpu == NULL) {
			printf("El cliente se desconectó\n");
			exit(1);
		}

		segment_read_write* segment_rw = deserialize_segment_read_write(package_cpu);

		switch (package_cpu->type) {
			case F_READ:
				sleep(memory_shared.mem_delay);
				char* buffer = read_memory(segment_rw->s_id, segment_rw->offset, segment_rw->size, memory_structure, segment_rw->pid);
				if (buffer == NULL) {
					// devolver seg_fault
					socket_send(socket_cpu, package_new(SEG_FAULT));
				} else {
					// devolver buffer
					socket_send(socket_cpu, serialize_message(buffer, false));
				}
				break;
			case F_WRITE:
				sleep(memory_shared.mem_delay);
				pthread_mutex_lock(&mutex_write);
				if (write_memory(segment_rw->s_id, segment_rw->offset, segment_rw->size, segment_rw->buffer, memory_structure, segment_rw->pid)) {
					// devolver ok
					socket_send(socket_cpu, package_new(OK_INSTRUCTION));
				} else {
					// devolver seg_fault
					socket_send(socket_cpu, package_new(SEG_FAULT));
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

void handle_kernel(int socket_kernel, memory_structure* memory_structure) {
	while (1) {
		t_package* package_kernel = socket_receive(socket_kernel);
		if (package_kernel == NULL) {
			printf("El cliente se desconectó\n");
			exit(1);
		}

		segment_table* sg = deserialize_segment_table(package_kernel);

		char* process_id = NULL;
		sprintf(process_id, "%d", sg->pid);

		switch (package_kernel->type) {
			case CREATE_PROCESS_MEMORY: {  // Creo la tabla de segmentos y la devuevlo al kernel cuando crea un proceso
				t_list* segment_table = create_sg_table(memory_structure, sg->pid);
				log_info(memory_config.logger, "Creación de Proceso PID: %s", process_id);
				// Envio la tabla de segmentos al kernel
				//socket_send(socket_kernel, serialize_segment_table(segment_table));	 // TODO: ver serializacion
				break;
			}
			case END_PROCCESS_MEMORY: {
				remove_sg_table(memory_structure, sg->pid);
				log_info(memory_config.logger, "Eliminación de Proceso PID: %s", process_id);
				break;
			}
			case CREATE_SEGMENT_MEMORY: {
				int flag = add_segment(memory_structure, sg->pid, sg->size_segment, sg->s_id);
				switch (flag) {
					case 1:
						// Devuelvo solicitud de compactacion
						socket_send(socket_kernel, package_new(COMPACT_MEMORY));
						break;
					case 2:
						// Devuelvo no hay espacio suficiente
						socket_send(socket_kernel, package_new(NO_SPACE_LEFT));
						break;
					default:
						// Devuelvo la base del segmento creado
						socket_send(socket_kernel, serialize_message((char*)&flag, false));
						break;
				}
				break;
			}
			case DELETE_SEGMENT_MEMORY:
				delete_segment(memory_structure, sg->pid, sg->s_id);
				break;
			case COMPACT_MEMORY:
				log_info(memory_config.logger, "Solicitud de compactación");
				sleep(memory_shared.com_delay);
				pthread_mutex_lock(&mutex_compact);
				compact_memory(memory_structure);
				pthread_mutex_unlock(&mutex_compact);
				break;

			default:
				log_error(memory_config.logger, "El proceso recibió algo indebido, finalizando modulo");
				exit(1);
				break;
		}
	}
}