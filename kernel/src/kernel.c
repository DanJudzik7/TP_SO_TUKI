#include "kernel.h"

int main(int argc, char** argv) {
	t_config* config = start_config("kernel");
	t_global_config_kernel* gck = new_global_config_kernel(config);
	t_helper_fs_handler* hfi = s_malloc(sizeof(t_helper_fs_handler));
	log_warning(gck->logger, "Iniciando el kernel");
	gck->alfa = config_get_int_value(config, "HRRN_ALFA");
	char* port = config_get_string_value(config, "PUERTO_ESCUCHA");
	gck->server_socket = socket_initialize_server(port);
	if (gck->server_socket == -1) {
		log_error(gck->logger, "No se pudo inicializar el socket de servidor");
		exit(EXIT_FAILURE);
	}
	log_warning(gck->logger, "Socket de servidor inicializado en puerto %s", port);

	int socket_cpu = connect_module(config, gck->logger, "CPU");
	gck->socket_memory = connect_module(config, gck->logger, "MEMORIA");
	hfi->socket_filesystem = connect_module(config, gck->logger, "FILESYSTEM");

	// Creación de Recursos: Diccionario de recursos -> [recurso,[instancias, [Cola de recursos] ] ]
	char** resources_helper = config_get_array_value(config, "RECURSOS");
	char** inst_resources_helper = config_get_array_value(config, "INSTANCIAS_RECURSOS");
	for (int i = 0; resources_helper[i] != NULL; i++) {
		log_info(gck->logger, "Cargando recurso %s con %s instancias", resources_helper[i], inst_resources_helper[i]);
		t_resource* resource = s_malloc(sizeof(t_resource));
		resource->assigned_to = NULL;
		resource->enqueued_processes = queue_create();
		resource->available_instances = atoi(inst_resources_helper[i]);
		dictionary_put(gck->resources, resources_helper[i], resource);
	}

	// Manejo de archivos y filesystem
	hfi->logger = gck->logger;
	hfi->global_files = dictionary_create();
	hfi->file_instructions = queue_create();
	pthread_t thread_fs;
	pthread_create(&thread_fs, NULL, (void*)handle_fs, hfi);

	// Manejo de consolas
	pthread_t thread_console;
	pthread_create(&thread_console, NULL, (void*)listen_consoles, gck);

	// Manejo de CPU y Short Term Scheduler
	while (1) {
		// Si no hay PCBs en la cola de activos, espero a que llegue uno
		// Organizo según el tipo de planificador
		t_pcb* pcb = short_term_scheduler(gck);
		if (pcb == NULL || pcb == 0) {
			sleep(1);
			continue;
		}
		log_debug(gck->logger, "Iniciando nuevo ciclo de ejecución %d del proceso %d", pcb->execution_context->program_counter, pcb->pid);
		log_warning(gck->logger, "PID: %d - Estado Anterior: READY - Estado Actual: EXEC", pcb->pid);
		pcb->state = EXEC;

		// Mando el PCB al CPU
		t_package* ec_package = serialize_execution_context(pcb->execution_context);
		if (!socket_send(socket_cpu, ec_package)) {
			log_error(gck->logger, "Error al enviar el Execution Context al CPU");
			break;
		}

		// Recibe el nuevo execution context
		t_package* package = socket_receive(socket_cpu);
		if (package != NULL && package->type == EXECUTION_CONTEXT)
			pcb->execution_context = deserialize_execution_context(package);
		else {
			log_error(gck->logger, "No se pudo recibir el Execution Context del proceso %d", pcb->pid);
			break;
		}

		t_instruction* kernel_request = pcb->execution_context->kernel_request;
		switch (kernel_request ? kernel_request->op_code : -1) {
			case I_O: {	 // time
				t_helper_pcb_io* hpi = s_malloc(sizeof(t_helper_pcb_io));
				hpi->pcb = pcb;
				hpi->logger = gck->logger;
				hpi->time = atoi(list_get(kernel_request->args, 0));
				pcb->state = BLOCK;
				log_warning(gck->logger, "PID: %d - Estado Anterior: READY - Estado Actual: BLOCK", pcb->pid);
				log_warning(gck->logger, "PID: %d - Bloqueado por: IO", pcb->pid);
				log_warning(gck->logger, "PID: %d - Ejecuta IO: %d", pcb->pid, hpi->time);
				pthread_t thread_io;
				pthread_create(&thread_io, NULL, (void*)handle_pcb_io, hpi);
				break;
			}
			case F_OPEN: {	// filename
				char* filename = list_get(kernel_request->args, 0);
				log_warning(gck->logger, "PID: %d - Abrir Archivo: %s", pcb->pid, filename);
				if (dictionary_has_key(hfi->global_files, filename)) {
					log_warning(gck->logger, "PID: %d - Estado Anterior: READY - Estado Actual: BLOCK", pcb->pid);
					log_warning(gck->logger, "PID: %d - Bloqueado por: %s", pcb->pid, filename);
					pcb->state = BLOCK;
					list_add(dictionary_get(hfi->global_files, filename), pcb);
					break;
				}
				if (!socket_send(hfi->socket_filesystem, serialize_instruction(kernel_request))) {
					log_error(gck->logger, "Error al enviar operación a filesystem");
					break;
				}
				t_package* package = socket_receive(hfi->socket_filesystem);
				if (package->type != MESSAGE_OK) {
					log_error(gck->logger, "Error al abrir archivo");
					break;
				}
				dictionary_put(hfi->global_files, filename, queue_create());
				dictionary_put(pcb->local_files, filename, 0);
				break;
			}
			case F_CLOSE: {	 // filename
				char* filename = list_get(kernel_request->args, 0);
				log_warning(gck->logger, "PID: %d - Cerrar Archivo: %s", pcb->pid, filename);
				if (!dictionary_has_key(hfi->global_files, filename)) {
					log_error(gck->logger, "El archivo %s no está abierto", filename);
					break;
				}
				/*if (!socket_send(hfi->socket_filesystem, serialize_instruction(kernel_request))) {
					log_error(gck->logger, "Error al enviar operación a filesystem");
					break;
				}
				t_package* package = socket_receive(hfi->socket_filesystem);
				if (package->type != MESSAGE_OK) {
					log_error(gck->logger, "Error al cerrar archivo");
					break;
				}*/
				t_queue* waiting_pcbs = dictionary_get(hfi->global_files, filename);
				if (queue_is_empty(waiting_pcbs)) {
					queue_destroy(waiting_pcbs);
					dictionary_remove(hfi->global_files, filename);
				} else
					((t_pcb*)queue_pop(waiting_pcbs))->state = READY;
				dictionary_remove(pcb->local_files, filename);
				break;
			}
			case F_SEEK: {	// filename, position
				char* filename = list_get(kernel_request->args, 0);
				char* position = list_get(kernel_request->args, 1);
				if (!dictionary_has_key(pcb->local_files, filename)) {
					log_error(gck->logger, "No se encontró el archivo %s", filename);
					break;
				}
				dictionary_put(pcb->local_files, filename, position);
				log_warning(gck->logger, "PID: %d - Actualizar puntero Archivo: %s - Puntero %s", pcb->pid, filename, position);
				break;
			}
			case F_TRUNCATE: {	// filename, bytes count
				if (!socket_send(hfi->socket_filesystem, serialize_instruction(kernel_request))) {
					log_error(gck->logger, "Error al enviar operación a filesystem");
					break;
				}
				t_package* package = socket_receive(hfi->socket_filesystem);
				if (package->type != MESSAGE_OK) {
					log_error(gck->logger, "Error al truncar archivo");
					break;
				}
				log_warning(gck->logger, "PID: %d - Truncar Archivo: %s - Tamaño: %s", pcb->pid, (char*)list_get(kernel_request->args, 0), (char*)list_get(kernel_request->args, 1));
				break;
			}
			case F_READ:  // filename, logical address, bytes count, segment, offset -> NAME(0) POS(1) SIZE(2) PID(3) S_ID(4) OFFSET(5)
			case F_WRITE: {
				char* filename = list_get(kernel_request->args, 0);
				// Se asegura de que el archivo esté abierto por el proceso
				if (!dictionary_has_key(pcb->local_files, filename)) {
					log_error(gck->logger, "El archivo %s no está abierto por el proceso %d", filename, pcb->pid);
					break;
				}
				t_file_instruction* fi = s_malloc(sizeof(t_file_instruction));
				fi->logicalAddress = atoi(list_get(kernel_request->args, 1));
				fi->pcb = pcb;
				fi->instruction = instruction_duplicate(kernel_request);
				list_replace(fi->instruction->args, 1, dictionary_get(pcb->local_files, filename));
				list_add_in_index(fi->instruction->args, 3, string_itoa(pcb->pid));
				queue_push(hfi->file_instructions, fi);
				pcb->state = BLOCK;
				log_warning(gck->logger, "PID: %d - Estado Anterior: READY - Estado Actual: BLOCK", pcb->pid);
				log_warning(gck->logger, "PID: %d - Bloqueado por: %s", pcb->pid, filename);
				break;
			}
			case CREATE_SEGMENT: {
				list_add(kernel_request->args, string_itoa(pcb->pid));
				kernel_request->op_code = MEM_CREATE_SEGMENT;
				if (!socket_send(gck->socket_memory, serialize_instruction(kernel_request))) {
					log_error(gck->logger, "Error al enviar instrucción a memoria");
					break;
				}
				t_package* package = socket_receive(gck->socket_memory);
				if (package->type == COMPACT_REQUEST) {
					if (!queue_is_empty(hfi->file_instructions)) log_warning(gck->logger, "Esperando Fin de Operaciones de FS");
					while (!queue_is_empty(hfi->file_instructions)) sleep(1);
					log_warning(gck->logger, "Compactación: Se solicitó compactación");
					t_instruction* mem_op = instruction_new(MEM_COMPACT_ALL);
					if (!socket_send(gck->socket_memory, serialize_instruction(mem_op))) {
						log_error(gck->logger, "Error al enviar operación a memoria");
						break;
					}
					free(mem_op);
					t_package* compact_package = socket_receive(gck->socket_memory);
					t_dictionary* segment_tables = deserialize_all_segments_tables(compact_package);
					// Cargar todos los nuevos segmentos
					for (int i = 0; i < list_size(gck->active_pcbs->elements); i++) {
						t_pcb* pcb = list_get(gck->active_pcbs->elements, i);
						list_destroy(pcb->execution_context->segments_table);
						pcb->execution_context->segments_table = dictionary_get(segment_tables, string_itoa(pcb->pid));
					}
					log_warning(gck->logger, "Se finalizó el proceso de compactación");
					// Volver a intentar
					if (!socket_send(gck->socket_memory, serialize_instruction(kernel_request))) {
						log_error(gck->logger, "Error al volver a enviar instrucción a memoria");
						break;
					}
					free(compact_package);
					package = socket_receive(gck->socket_memory);
				}
				if (package->type == COMPACT_REQUEST) {
					log_error(gck->logger, "Error: Solicitud de compactación consecutiva");
				} else if (package->type == OUT_OF_MEMORY) {
					log_warning(gck->logger, "Finaliza el proceso %d - Motivo: OUT_OF_MEMORY", pcb->pid);
					log_warning(gck->logger, "PID: %d - Estado Anterior: READY - Estado Actual: EXIT", pcb->pid);
					pcb->state = EXIT_PROCESS;
				} else if (package->type == MESSAGE_OK) {
					char* s_base = deserialize_message(package);
					t_segment* segment = s_malloc(sizeof(t_segment));
					segment->base = atoi(s_base);
					segment->s_id = atoi((char*)list_get(kernel_request->args, 0));
					segment->offset = atoi((char*)list_get(kernel_request->args, 1));
					list_add(pcb->execution_context->segments_table, segment);
					log_warning(gck->logger, "PID: %d - Crear Segmento - Id: %s - Tamaño: %s", pcb->pid, (char*)list_get(kernel_request->args, 0), (char*)list_get(kernel_request->args, 1));
					free(s_base);
				} else
					log_error(gck->logger, "Error desconocido al crear segmento");
				break;
			}
			case DELETE_SEGMENT: {
				list_add(kernel_request->args, string_itoa(pcb->pid));
				kernel_request->op_code = MEM_DELETE_SEGMENT;
				if (!socket_send(gck->socket_memory, serialize_instruction(kernel_request))) {
					log_error(gck->logger, "Error al enviar instrucción a memoria");
					break;
				}
				t_package* package = socket_receive(gck->socket_memory);
				if (package == NULL || package->type != OK_INSTRUCTION) log_error(gck->logger, "Error al eliminar segmento");
				log_warning(gck->logger, "PID: %d - Eliminar Segmento - Id Segmento: %s", pcb->pid, (char*)list_get(kernel_request->args, 0));
				break;
			}
			case YIELD: {
				if (gck->algorithm_is_hrrn) {
					// Si nunca se ejecuto antes, va a tener un ultimo tiempo de ráfaga en 0
					if (pcb->last_burst_time == 0)
						// aprox nueva ráfaga =  α . estimador inicial + (1 - α) . ráfagaAnterior
						pcb->aprox_burst_time = (gck->alfa * gck->default_burst_time) + ((1 - gck->alfa) * pcb->execution_context->last_burst_time);
					else
						// aprox nueva ráfaga =  α . ultima aproximación de ráfaga + (1 - α) . ráfagaAnterior
						pcb->aprox_burst_time = (gck->alfa + pcb->aprox_burst_time) + ((1 - gck->alfa) * pcb->execution_context->last_burst_time);
				}
				break;
			}
			case WAIT: {
				char* resource_name = list_get(kernel_request->args, 0);
				t_resource* resource = resource_get(pcb, gck, resource_name);
				if (resource == NULL) break;
				if (resource->available_instances > 0) {
					resource->assigned_to = pcb;
					resource->available_instances--;
					log_warning(gck->logger, "PID: %d - Wait: %s - Instancias: %d", pcb->pid, resource_name, resource->available_instances);
					gck->prioritized_pcb = pcb;
				} else {
					pcb->state = BLOCK;
					log_warning(gck->logger, "PID: %d - Estado Anterior: READY - Estado Actual: BLOCK", pcb->pid);
					log_warning(gck->logger, "PID: %d - Bloqueado por: %s", pcb->pid, resource_name);
					log_info(gck->logger, "El proceso %d se bloqueó ya que el recurso no está disponible aún", pcb->pid);
					queue_push(resource->enqueued_processes, pcb);
				}
				break;
			}
			case SIGNAL: {
				char* resource_name = list_get(kernel_request->args, 0);
				t_resource* resource = resource_get(pcb, gck, resource_name);
				if (resource == NULL) break;
				/*if (resource->assigned_to != pcb) {
					log_error(gck->logger, "El proceso %d intentó liberar un recurso que no tiene asignado", pcb->pid);
					log_warning(gck->logger, "PID: %d - Estado Anterior: READY - Estado Actual: EXIT", pcb->pid);
					pcb->state = EXIT_PROCESS;
					break;
				}*/
				gck->prioritized_pcb = pcb;
				resource_signal(resource, resource_name, gck->logger);
				break;
			}
			case EXIT: {
				if (list_size(kernel_request->args) > 0)
					log_warning(gck->logger, "Finaliza el proceso %d - Motivo: SEG_FAULT", pcb->pid);
				else
					log_warning(gck->logger, "Finaliza el proceso %d - Motivo: SUCCESS", pcb->pid);
				pcb->state = EXIT_PROCESS;
				break;
			}
		}
		if (kernel_request != NULL) {
			instruction_destroy(kernel_request);
			pcb->execution_context->kernel_request = NULL;
		}
		queue_push(gck->active_pcbs, pcb);
		if (pcb->state == EXEC) {
			pcb->state = READY;
			log_warning(gck->logger, "PID: %d - Estado Anterior: EXEC - Estado Actual: READY", pcb->pid);
		}
		if (pcb->state == EXIT_PROCESS) {
			log_debug(gck->logger, "Al finalizar %d, ejecutó hasta la instrucción %d", pcb->pid, pcb->execution_context->program_counter - 1);
			long_term_schedule(gck);
		} else
			log_debug(gck->logger, "Se ha completado hasta la instrucción %d de %d", pcb->execution_context->program_counter - 1, pcb->pid);
	}

	log_warning(gck->logger, "Finalizando el kernel. Se desconectó un módulo esencial.");
	log_destroy(gck->logger);
	config_destroy(config);
	return 0;
}