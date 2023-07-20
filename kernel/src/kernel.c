#include "kernel.h"

int main(int argc, char** argv) {
	t_config* config = start_config("kernel");
	t_global_config_kernel* gck = new_global_config_kernel(config);
	t_helper_file_instruction* hfi = s_malloc(sizeof(t_helper_file_instruction));
	log_warning(gck->logger, "Iniciando el kernel");
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
		log_warning(gck->logger, "Iniciando nuevo ciclo de ejecución del proceso %d", pcb->pid);
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
				pcb->state = BLOCK;
				t_helper_pcb_io* hpi = s_malloc(sizeof(t_helper_pcb_io));
				hpi->pcb = pcb;
				hpi->logger = gck->logger;
				hpi->time = atoi(list_get(kernel_request->args, 0));
				pthread_t thread_io;
				pthread_create(&thread_io, NULL, (void*)handle_pcb_io, hpi);
				log_warning(gck->logger, "Proceso %d bloqueado en I/O por %d segundos", pcb->pid, hpi->time);
				break;
			}
			case F_OPEN: {	// filename
				char* filename = list_get(kernel_request->args, 0);
				if (dictionary_has_key(hfi->global_files, filename)) {
					log_error(gck->logger, "Error: El archivo %s ya está abierto", filename);
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
				if (!dictionary_has_key(hfi->global_files, filename)) {
					log_error(gck->logger, "El archivo %s no está abierto", filename);
					break;
				}
				if (!socket_send(hfi->socket_filesystem, serialize_instruction(kernel_request))) {
					log_error(gck->logger, "Error al enviar operación a filesystem");
					break;
				}
				t_package* package = socket_receive(hfi->socket_filesystem);
				if (package->type != MESSAGE_OK) {
					log_error(gck->logger, "Error al cerrar archivo");
					break;
				}
				t_queue* waiting_pcbs = dictionary_get(hfi->global_files, filename);
				if (queue_is_empty(waiting_pcbs)) {
					list_destroy(dictionary_get(hfi->global_files, filename));
					dictionary_remove(hfi->global_files, filename);
				} else
					((t_pcb*)queue_pop(waiting_pcbs))->state = READY;
				dictionary_remove(pcb->local_files, filename);
				break;
			}
			case F_SEEK: {	// filename, position
				char* filename = list_get(kernel_request->args, 0);
				char* position = list_get(kernel_request->args, 1);
				if (dictionary_has_key(pcb->local_files, filename)) {
					dictionary_put(pcb->local_files, filename, position);
					log_info(gck->logger, "Se movió el puntero de %s a %s", filename, position);
				} else
					log_error(gck->logger, "No se encontró el archivo %s", filename);
				break;
			}
			case F_TRUNCATE: {	// filename, bytes count
				if (!socket_send(hfi->socket_filesystem, serialize_instruction(kernel_request))) {
					log_error(gck->logger, "Error al enviar operación a filesystem");
					break;
				}
				t_package* package = socket_receive(hfi->socket_filesystem);
				log_error(gck->logger, "El truncate del archivo fue %s", package->type == MESSAGE_OK ? "exitosa" : "fallida");
				break;
			}
			case F_READ:  // filename, logical address, bytes count, segment, offset -> NAME(0) posicion(1) tamaño_leer(2) PID(3) S_ID(4) OFFSET(5)
			case F_WRITE: {
				char* filename = list_get(kernel_request->args, 0);
				t_instruction* fs_op = instruction_duplicate(kernel_request);
				list_replace(fs_op->args, 1, dictionary_get(pcb->local_files, filename));
				list_add_in_index(fs_op->args, 3, string_itoa(pcb->pid));
				// Se asegura de que el archivo esté abierto por el proceso
				if (!dictionary_has_key(pcb->local_files, filename)) {
					log_error(gck->logger, "El archivo %s no está abierto por el proceso %d", filename, pcb->pid);
					break;
				}
				queue_push(hfi->file_instructions, fs_op);
				pcb->state = BLOCK;
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
					log_info(gck->logger, "Solicitud de compactación");
					while (!queue_is_empty(hfi->file_instructions)) sleep(1);
					log_info(gck->logger, "Comenzando compactación");
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
					if (!socket_send(gck->socket_memory, serialize_instruction(kernel_request))) {
						log_error(gck->logger, "Error al volver a enviar instrucción a memoria");
						break;
					}
					free(compact_package);
					package = socket_receive(gck->socket_memory);
				}
				if (package->type == COMPACT_REQUEST) {
					log_error(gck->logger, "Error: Solicitud de compactación consecutiva");
				} else if (package->type == NO_SPACE_LEFT) {
					log_error(gck->logger, "Error del kernel: Out of Memory");
					pcb->state = EXIT_PROCESS;
				} else if (package->type == MESSAGE_OK) {
					char* s_base = deserialize_message(package);
					log_info(gck->logger, "El segmento se creó con base %s", s_base);
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
				if (package->type != OK_INSTRUCTION) log_error(gck->logger, "Error al eliminar segmento");
				break;
			}
			case YIELD: {
				if (gck->algorithm_is_hrrn) {
					// TODO: calcular un nuevo estimado para su próxima ráfaga utilizando la fórmula de promedio ponderado vista en clases.
					// if(gck->algorithm_is_hrrn)
					// pcb->aprox_burst_time =
				}
				break;
			}
			case WAIT: {
				t_resource* resource = resource_get(pcb, gck, list_get(kernel_request->args, 0));
				if (resource == NULL) break;
				if (resource->available_instances >= 0) {
					resource->assigned_to = pcb;
					resource->available_instances--;
					log_info(gck->logger, "Las instancias del recurso se redujeron a %i", resource->available_instances);
					gck->prioritized_pcb = pcb;
				} else {
					pcb->state = BLOCK;
					queue_push(resource->enqueued_processes, pcb);
				}
				break;
			}
			case SIGNAL: {
				t_resource* resource = resource_get(pcb, gck, list_get(kernel_request->args, 0));
				if (resource == NULL) break;
				if (resource->assigned_to != pcb) {
					log_error(gck->logger, "El proceso %d intentó usar un recurso que no tiene asignado", pcb->pid);
					pcb->state = EXIT_PROCESS;
					break;
				}
				gck->prioritized_pcb = pcb;
				resource_signal(resource, gck->logger);
				break;
			}
			case EXIT: {
				pcb->state = EXIT_PROCESS;
				break;
			}
		}
		log_warning(gck->logger, "Se ha completado el ciclo de instrucción de %d", pcb->pid);
		if (pcb->state == EXEC) pcb->state = READY;
		if (pcb->state == EXIT_PROCESS) long_term_schedule(gck);
		queue_push(gck->active_pcbs, pcb);
		if (kernel_request != NULL) {
			instruction_destroy(kernel_request);
			pcb->execution_context->kernel_request = NULL;
		}
	}

	log_warning(gck->logger, "Finalizando el kernel. Se desconectó un módulo esencial.");
	return 0;
}