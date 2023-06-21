#include "kernel.h"

int main(int argc, char** argv) {
	t_log* logger = start_logger("kernel");
	t_config* config = start_config("kernel");
	log_warning(logger, "Iniciando el kernel");
	char* port = config_get_string_value(config, "PUERTO_ESCUCHA");	 // Obtenemos el puerto con el que escucharemos conexiones
	int socket_kernel = socket_initialize_server(port);				 // Inicializo el socket en el puerto cargado por la config
	if (socket_kernel == -1) {
		log_error(logger, "No se pudo inicializar el socket de servidor");
		exit(EXIT_FAILURE);
	}
	log_warning(logger, "Socket de servidor inicializado en puerto %s", port);

	// Connect_module conecta al modulo que le pasa como tercer parámetros
	int socket_cpu = connect_module(config, logger, "CPU");
	// int socket_memoria = connect_module(config, logger, "MEMORIA");
	// int socket_filesystem = connect_module(config, logger, "FILESYSTEM");

	// Obtengo del config el algoritmo a usar
	char* algorithm = config_get_string_value(config, "ALGORITMO_PLANIFICACION");
	bool algorithm_is_hrrn = strcmp(algorithm, "HRRN") == 0;
	if (!algorithm_is_hrrn && strcmp(algorithm, "FIFO") != 0) {
		log_error(logger, "El algoritmo de planificación no es válido");
		exit(EXIT_FAILURE);
	}
	log_warning(logger, "El algoritmo de planificación es: %s", algorithm);
	// To do: flags mutex para manejo de estos listas
	t_global_config_kernel* gck = s_malloc(sizeof(t_global_config_kernel));
	gck->logger = logger;
	gck->new_pcbs = queue_create();		// Cola local de PCBs en NEW
	gck->active_pcbs = queue_create();	// Cola global de PCBs en READY, EXEC y BLOCK
	gck->connection_kernel = socket_kernel;  
	//TODO: ESTO es realmente int no un string_value
	gck->max_multiprogramming = *config_get_string_value(config, "GRADO_MAX_MULTIPROGRAMACION") - '0';
	gck->default_burst_time = *config_get_string_value(config, "ESTIMACION_INICIAL") - '0';
	gck->algorithm_is_hrrn = algorithm_is_hrrn;
	gck->pcb_priority_helper = NULL;
	// CREACION DE RECURSOS
	// Diccionario de recursos -> [recurso,[instancias, [Cola de recursos] ] ]
	t_dictionary *resources_dict = dictionary_create();
	char** resources_helper = config_get_array_value(config, "RECURSOS");
	char** inst_resources_helper = config_get_array_value(config, "INSTANCIAS_RECURSOS");
	for (int i = 0; resources_helper[i] != NULL; i++) {
		printf("cargando de recurso un %s con valor -> %s",resources_helper[i], inst_resources_helper[i]);
		resources_table* resource_table_handler = s_malloc(sizeof(resources_handler));
		t_queue* queue_resource = queue_create();
		resource_table_handler->instances = (int)atoi(inst_resources_helper[i]);
		resource_table_handler->resource_queue = queue_resource;
		dictionary_put(resources_dict, resources_helper[i], resource_table_handler);
	}
	gck->resources = resources_dict;
	sem_init(&(gck->flag_with_pcb), 0, 1);

	// Manejo de consolas
	pthread_t thread_consola;
	pthread_create(&thread_consola, NULL, (void*)listen_consoles, gck);

	int sem_pcb_value;
	
	// Manejo de CPU y Short Term Scheduler. Antes era listener_cpu.
	while (1) {
		//PARA QUE PUSE ESTA FLAG? NI DIOS SABE PORQUE pero recuerdo que era algo importante
	sem_getvalue(&gck->flag_with_pcb, &sem_pcb_value);
	if (sem_pcb_value==1) {
		sem_wait(&gck->flag_with_pcb);
		// Si no hay PCBs en la cola de activos, espero a que llegue uno
		// Organizo según el tipo de planificador
		t_pcb* pcb = short_term_scheduler(gck);
		
		if (pcb == NULL || pcb == 0) {
			sem_post(&gck->flag_with_pcb);
			continue;
		};
		log_warning(logger, "-----------------------Tenemos un nuevo PCB----------------------");
		pcb->state = EXEC;
		// Mando el PCB al CPU
		log_warning(logger, "----------------------Enviando context al CPU-----------------------");
		print_execution_context(pcb->execution_context);
		t_package* ec_package = serialize_execution_context(pcb->execution_context);
		// Recibe el nuevo execution context, que puede estar en EXIT o BLOCK
		if (!socket_send(socket_cpu, ec_package)) {
		sem_post(&gck->flag_with_pcb);
			break;
		};

		// Recibe el nuevo execution context
		t_package* package = socket_receive(socket_cpu);
		if (package != NULL && package->type == EXECUTION_CONTEXT) {
			pcb->execution_context = deserialize_execution_context(package);
			log_warning(logger, "----------------------Recibiendo context %d al CPU----------------------", pcb->pid);
			print_execution_context(pcb->execution_context);
		} else {
			log_warning(logger, "No se pudo recibir el Execution Context del proceso %d", pcb->pid);
			sem_post(&gck->flag_with_pcb);
		}
		// Actualizo el estado del PCB según el nuevo execution context
		pcb->state = pcb->execution_context->updated_state;

		// Nota: EXIT es solo finalización implícita del proceso (según la consigna, usuario y error). El completado de instrucciones debe devolver READY.
		//TODO: RELEER t.u.k.i y ver si hay que hacer algo con el PCB 
		if (no_more_instructions(pcb->execution_context) && pcb->state != EXIT_PROCESS){
			log_warning(logger, "Bloqueando PCB, ya no posee mas instrucciones");
			pcb->state = BLOCK;
			sem_post(&gck->flag_with_pcb);
		} 
		else {
			switch (pcb->state)
			{
			case YIELD:
				state_yield(pcb,gck);
				break;
			case SIGNAL:
			case WAIT:
				state_signal_or_wait(pcb,gck);
				break;
			case EXIT_PROCESS: 
				state_exit_process(pcb,gck);
				break;
			default:
				queue_push(gck->active_pcbs, pcb);
				log_warning(logger, "-----------------------Guardando PCB %d en cola de READY-----------------------", pcb->pid);
				sem_post(&gck->flag_with_pcb);
				break;
			}	
		}	
	}
	else continue;
	}

	log_warning(logger, "Finalizando el kernel. Se desconectó un módulo esencial.");
	return 0;
}
