#include "planificador.h"

//ORGANIZA LOS PCB SEGUN ME LLEGAN, PUEDE ENVIARLO AL SHORT O LONG o manejarlos internamente por si llegan bloqueados. 
void planificador_kernel(process* process) {
	
}

void reciver_new_pcb(config_current_process* current_config_process) {
	t_pcb* new_pcb = create_pcb(current_config_process -> current_process , current_config_process -> global_config_kernel -> logger, current_config_process -> global_config_kernel -> default_burst_time);
	log_info(current_config_process -> global_config_kernel -> logger, "Se crea el proceso %d en NEW", new_pcb -> pid);
	current_config_process -> current_process -> pcb = new_pcb;
	long_term_scheduler(current_config_process);
}

op_code_reception* long_term_scheduler(config_current_process* current_config_process) {
	log_info(current_config_process->global_config_kernel->logger,"LLego long_term_scheduler...");

	log_info(current_config_process->global_config_kernel->logger,"Paso process");
	global_config_kernel* gck = malloc(sizeof(global_config_kernel));

	log_info(gck->logger,"Paso pck");

	log_info(gck->logger,"Iniciando planificador...");
	log_error(gck ->logger,"Proceso en estado: %d",current_config_process->current_process->pcb->state_pcb);
	
	sem_t* FLAG_MULTIPROGRAMACION = malloc(sizeof(sem_t));
	sem_init(&FLAG_MULTIPROGRAMACION,1 ,gck -> max_multiprogramming); //Debe cargarse desde config
	t_log* logger = current_config_process->global_config_kernel -> logger;
	t_queue* global_ready_pcb = current_config_process->global_config_kernel -> global_pcb;
	

	//Creo la cola local de PCB en estado |NEW|
	//seguramente debe enviarse al planificador como algo global, pero solo la usa este
	log_info(current_config_process->global_config_kernel->logger,"pasa el sem");
	t_queue* queue_local_pcb_new = queue_create(); // TODO: LA COLA YA DEBERIA EXISTIR, NO CREARSE ACA

	log_info(current_config_process->global_config_kernel->logger,"pasa el create");
	
	log_info(current_config_process->global_config_kernel ->logger,"Proceso en estado: %d",current_config_process->current_process->pcb->state_pcb);
	if( current_config_process->current_process->pcb->state_pcb == NEW_PROCESS ){
		log_info(gck->logger,"entra al if");

		queue_push(queue_local_pcb_new, current_config_process->current_process);
		log_info(logger, "El proceso %d se agrego a la cola de espera de NEW", current_config_process->current_process -> pcb -> pid);
	}
	log_info(gck->logger,"pasa el if");
	
	//TODO: Deberia ser un while que verifique todo el tiempo si puede ejecutar un nuevo pcb
	//Reviso si la multiprogramacion permite planificar mas de uno
	int sem_value;
	sem_getvalue(&FLAG_MULTIPROGRAMACION, &sem_value);
	while(sem_value < 0){
		//consumo 1 del flag de multiprogramacion
		sem_wait(&FLAG_MULTIPROGRAMACION);

		//agarro el ultimo pcb en new y lo mando a ready 
		t_pcb* last_pcb = queue_peek(queue_local_pcb_new);  
		//Mando el pcb a MEMORIA para que me devuelva las estructuras necesarias para inicializar, a modificar & implementar memoria
		/*
			if( memoria_recived == OK )
			last_pcb -> table_open_files = memoria.serializado()
		*/	 
		last_pcb -> state_pcb = READY;
		//Lo mando a la cola global de procesos |READY| 
		queue_push(global_ready_pcb, last_pcb);
		//Ejecuto el planificador corto plazo
		// ----------> short_term_scheduler(ci);
		//Lo saco de la cola local de procesos |NEW|
		queue_pop(queue_local_pcb_new);

	}
	
	if( current_config_process->current_process -> pcb -> state_pcb == EXIT){
		log_info(logger, "El proceso %d termino su ejecucion", current_config_process->current_process -> pcb -> pid);
		// libero 1 del flag de multiprogramacion
		sem_post(&FLAG_MULTIPROGRAMACION);
		//BORRAR ID DE EJECUCION
		//liberar recursos de pcb 
		// devolver motivo de finalizacion
	}

	return OK;
}

//Este planificador de corto plazo recibe un pcb, algoritmo de planificacion, y la cola global de pcb |READY|, logger, int server_cpu
void short_term_scheduler(global_config_kernel* gck, process* process) {

	//DETECTAMOS EL ESTADO ACTUAL DE EJECUCION, SI NO SE ESTA EJECUTANDO NADA HAGO LO DE ABAJO
	/*
		if (strcmp(gck -> algorithm, "FIFO") == 0) {  // Organizo según el tipo de planificador
			pcb_send = fifo(gck -> queue_global_pcb, process -> pcb );
		} else if (strcmp(gck -> algorithm, "HRRN") == 0) {
			pcb_send = hrrn(gck -> logger, queue_global_pcb, process -> pcb);
		}
		// mando a cpu, y espera
		// si  de cpu no vuelve que es exit: se vuelve a agregar a la queue
	*/
}