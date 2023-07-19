#include "kernel.h"

int main(int argc, char** argv) {
	t_config* config = start_config("kernel");
	t_global_config_kernel* gck = new_global_config_kernel(config);
	t_helper_file_instruction* hfi = s_malloc(sizeof(t_helper_file_instruction));
	log_warning(gck->logger, "Iniciando el kernel");
	char* port = config_get_string_value(config, "PUERTO_ESCUCHA");
	int socket_kernel = socket_initialize_server(port);
	if (socket_kernel == -1) {
		log_error(gck->logger, "No se pudo inicializar el socket de servidor");
		exit(EXIT_FAILURE);
	}
	log_warning(gck->logger, "Socket de servidor inicializado en puerto %s", port);

	int socket_cpu = connect_module(config, gck->logger, "CPU");
	gck->socket_memory = connect_module(config, gck->logger, "MEMORIA");
	hfi->socket_filesystem = connect_module(config, gck->logger, "FILESYSTEM");

	// To do: flags mutex para manejo de estos listas
	// Creación de Recursos: Diccionario de recursos -> [recurso,[instancias, [Cola de recursos] ] ]
	char** resources_helper = config_get_array_value(config, "RECURSOS");
	char** inst_resources_helper = config_get_array_value(config, "INSTANCIAS_RECURSOS");
	for (int i = 0; resources_helper[i] != NULL; i++) {
		printf("Cargando de recurso un %s con valor -> %s", resources_helper[i], inst_resources_helper[i]);
		t_resource* resource = s_malloc(sizeof(t_resource));
		resource->enqueued_processes = queue_create();
		resource->available_instances = atoi(inst_resources_helper[i]);
		dictionary_put(gck->resources, resources_helper[i], resource);
	}

	// Manejo de archivos y filesystem
	t_list* open_files = list_create();
	hfi->logger = gck->logger;
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

		if (pcb == NULL || pcb == 0) continue;
		log_warning(gck->logger, "-----------------------Tenemos un nuevo PCB----------------------");
		pcb->state = EXEC;
		// Mando el PCB al CPU
		log_warning(gck->logger, "----------------------Enviando context al CPU-----------------------");
		print_execution_context(pcb->execution_context);
		t_package* ec_package = serialize_execution_context(pcb->execution_context);
		// Recibe el nuevo execution context, que puede estar en EXIT o BLOCK
		if (!socket_send(socket_cpu, ec_package)) {
			log_error(gck->logger, "Error al enviar el Execution Context al CPU");
			break;
		}

		// Recibe el nuevo execution context
		t_package* package = socket_receive(socket_cpu);
		if (package != NULL && package->type == EXECUTION_CONTEXT) {
			pcb->execution_context = deserialize_execution_context(package);
			log_warning(gck->logger, "----------------------Recibiendo context %d al CPU----------------------", pcb->pid);
			print_execution_context(pcb->execution_context);
		} else {
			log_warning(gck->logger, "No se pudo recibir el Execution Context del proceso %d", pcb->pid);
		}

		// Nota: EXIT es solo finalización implícita del proceso (según la consigna, usuario y error). El completado de instrucciones debe devolver READY.
		// TODO: RELEER t.u.k.i y ver si hay que hacer algo con el PCB
		if (pcb->execution_context->program_counter >= list_size(pcb->execution_context->instructions->elements) && pcb->state != EXIT_PROCESS) {
			log_warning(gck->logger, "Bloqueando PCB, ya no posee mas instrucciones");
			pcb->state = BLOCK;
		}
		t_instruction* kernel_request = pcb->execution_context->kernel_request;
		if (kernel_request == NULL) continue;
		switch (kernel_request->op_code) {
			case I_O: {	 // time
				pcb->state = BLOCK;
				t_helper_pcb_io* hpi = s_malloc(sizeof(t_helper_pcb_io));
				hpi->pcb = pcb;
				hpi->time = atoi(list_get(kernel_request->args, 0));
				pthread_t thread_io;
				pthread_create(&thread_io, NULL, (void*)handle_pcb_io, hpi);
				log_info(gck->logger, "Proceso %d bloqueado en I/O por %d segundos", pcb->pid, hpi->time);
				break;
			}
			case F_OPEN: {	// filename
				char* filename = list_get(kernel_request->args, 0);
				if (is_in_list(open_files, filename)) {
					log_error(gck->logger, "El archivo %s ya está abierto", filename);
					continue;
				}
				if (!socket_send(hfi->socket_filesystem, serialize_instruction(kernel_request))) {
					log_error(gck->logger, "Error al enviar operación a filesystem");
					continue;
				}
				t_package* package = socket_receive(hfi->socket_filesystem);
				if (package->type != MESSAGE_OK) {
					log_error(gck->logger, "Error al abrir archivo");
					continue;
				}
				list_add(open_files, filename);
				dictionary_put(pcb->files, filename, 0);
				break;
			}
			case F_CLOSE: {	 // filename
				char* filename = list_get(kernel_request->args, 0);
				if (!is_in_list(open_files, filename)) {
					log_error(gck->logger, "El archivo %s no está abierto", filename);
					continue;
				}
				if (!socket_send(hfi->socket_filesystem, serialize_instruction(kernel_request))) {
					log_error(gck->logger, "Error al enviar operación a filesystem");
					continue;
				}
				t_package* package = socket_receive(hfi->socket_filesystem);
				if (package->type != MESSAGE_OK) {
					log_error(gck->logger, "Error al cerrar archivo");
					continue;
				}
				list_remove_element(open_files, filename);
				dictionary_remove(pcb->files, filename);
				break;
			}
			case F_SEEK: {	// filename, position
				char* filename = list_get(kernel_request->args, 0);
				int pos = atoi(list_get(kernel_request->args, 0));
				if (dictionary_has_key(pcb->files, filename)) {
					dictionary_put(pcb->files, filename, pos);
					log_info(gck->logger, "Se movió el puntero de %s a %d", filename, pos);
				} else {
					log_error(gck->logger, "No se encontró el archivo %s", filename);
				}
				break;
			}
			case F_TRUNCATE: {	// filename, bytes count
				if (!socket_send(hfi->socket_filesystem, serialize_instruction(kernel_request))) {
					log_error(gck->logger, "Error al enviar operación a filesystem");
					return;
				}
				t_package* package = socket_receive(hfi->socket_filesystem);
				printf("El truncate del archivo fue %s", package->type == MESSAGE_OK ? "exitosa" : "fallida");
				break;
			}
			case F_READ:  // filename, logical address, bytes count, segment, offset -> NAME(0) posicion(1) tamaño_leer(2) PID(3) S_ID(4) OFFSET(5)
			case F_WRITE: {
				t_instruction* fs_op = instruction_duplicate(kernel_request);
				list_replace(fs_op->args, 1, dictionary_get(pcb->files, list_get(fs_op->args, 0)));
				list_add_in_index(fs_op->args, 3, pcb->pid);
				queue_push(hfi->file_instructions, fs_op);
				pcb->state = BLOCK;
				break;
			}
			case CREATE_SEGMENT: {
				list_add(kernel_request->args, pcb->pid);
				kernel_request->op_code = MEM_CREATE_SEGMENT;
				if (!socket_send(gck->socket_memory, serialize_instruction(kernel_request))) {
					log_error(gck->logger, "Error al enviar instrucción a memoria");
				}
				t_package* package = socket_receive(gck->socket_memory);
				if (package->type == COMPACT_REQUEST) {
					log_error(gck->logger, "Solicitud de compactación");
					while (!queue_is_empty(hfi->file_instructions)) sleep(1);
					t_instruction* mem_op = instruction_new(MEM_COMPACT_ALL);
					if (!socket_send(gck->socket_memory, serialize_instruction(mem_op))) {
						log_error(gck->logger, "Error al enviar operación a memoria");
						continue;
					}
					free(mem_op);
					t_package* package = socket_receive(gck->socket_memory);
					t_dictionary* segment_tables = deserialize_all_segments_tables(package);
					// Cargar todos los nuevos segmentos
					for (int i = 0; i < list_size(gck->active_pcbs->elements); i++) {
						t_pcb* pcb = list_get(gck->active_pcbs->elements, i);
						list_destroy(pcb->execution_context->segments_table);
						pcb->execution_context->segments_table = dictionary_get(segment_tables, pcb->pid);
					}
					if (!socket_send(gck->socket_memory, serialize_instruction(kernel_request))) {
						log_error(gck->logger, "Error al volver a enviar instrucción a memoria");
					}
					free(package);
					continue;
				}
				if (package->type == NO_SPACE_LEFT) {
					log_error(gck->logger, "Error del kernel: Out of Memory");
					exit_process(pcb, gck);
					continue;
				}
				if (package->type == MESSAGE_OK) {
					char* s_base = deserialize_message(package);
					log_error(gck->logger, "Error desconocido al leer el archivo");
					continue;
				}
				log_error(gck->logger, "Error desconocido al leer el archivo");
				break;
			}
			case DELETE_SEGMENT: {
				list_add(kernel_request->args, pcb->pid);
				kernel_request->op_code = MEM_DELETE_SEGMENT;
				if (!socket_send(gck->socket_memory, serialize_instruction(kernel_request))) {
					log_error(gck->logger, "Error al enviar instrucción a memoria");
				}
				t_package* package = socket_receive(gck->socket_memory);
				if (package->type != OK_INSTRUCTION) {
					log_error(gck->logger, "Error al eliminar segmento");
					return;
				}
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
				if (resource->available_instances >= 0) {
					resource->available_instances--;
					log_info(gck->logger, "Las instancias del recurso se redujeron a -> %i", resource->available_instances);
					gck->prioritized_pcb = pcb;
				} else {
					pcb->state = BLOCK;
					queue_push(resource->enqueued_processes, pcb);
				}
				break;
			}
			case SIGNAL: {
				t_resource* resource = resource_get(pcb, gck, list_get(kernel_request->args, 0));
				resource->available_instances++;
				log_info(gck->logger, "Las instancias del recurso aumentaron a %i", resource->available_instances);
				t_pcb* pcb_priority = queue_pop(resource->enqueued_processes);
				pcb_priority->state = READY;
				gck->prioritized_pcb = pcb;
				break;
			}
			case EXIT: {
				exit_process(pcb, gck);
				break;
			}
		}
		log_warning(gck->logger, "-----------------------Guardando PCB %d en cola de READY-----------------------", pcb->pid);
		queue_push(gck->active_pcbs, pcb);
		instruction_delete(kernel_request);
	}

	log_warning(gck->logger, "Finalizando el kernel. Se desconectó un módulo esencial.");
	return 0;
}