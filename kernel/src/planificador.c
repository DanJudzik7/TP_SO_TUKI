#include "planificador.h"

//ORGANIZA LOS PCB SEGUN ME LLEGAN, PUEDE ENVIARLO AL SHORT O LONG o manejarlos internamente por si llegan bloqueados. 
void planificador_kernel(t_console_init* ci) {
	log_info(ci->logger, "llego");

	

}

op_code_reception* long_term_scheduler(t_console_init* ci) {

	/*
	typedef struct {
	t_log* logger;
	int console;
	t_queue* global_pcb;
	char* algorithm;
	int default_burst_time;
	int server_fd;
} t_console_init;
*/
	int FLAG_MULTIPROGRAMACION = 1;
	t_log* logger = ci -> logger;
	t_queue* global_ready_pcb = ci -> global_pcb;
	
	//Creo la cola local de PCB en estado |NEW|
	//seguramente debe enviarse al planificador como algo global, pero solo la usa este
	t_queue* queue_local_pcb_new = queue_create();

	//Creo el PCB con los datos que me llegan de la consola
	t_pcb* new_pcb = malloc(sizeof(t_pcb));
	new_pcb = create_pcb(ci);
	//Lo inserto en la cola local de PCB en estado |NEW|
	queue_push(queue_local_pcb_new, new_pcb);	  
	 
	//Reviso si la multiprogramacion permite planificar mas de uno
	if(!FLAG_MULTIPROGRAMACION < 0){
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
		short_term_scheduler(ci);
		//Lo saco de la cola local de procesos |NEW|
		queue_pop(queue_local_pcb_new);
	}
	
	if( ci-> pcb -> execution_context -> state_pcb == EXIT){
		log_info(logger, "El proceso %d termino su ejecucion", ci -> pcb -> pid);
	//BORRAR ID DE EJECUCION
	//liberar recursos de pcb 
		// devolver motivo de finalizacion
	}

	return OK;
}

//Este planificador de corto plazo recibe un pcb, algoritmo de planificacion, y la cola global de pcb |READY|, logger, int server_cpu
void short_term_scheduler(t_console_init* ci) {

	//DETECTAMOS EL ESTADO ACTUAL DE EJECUCION, SI NO SE ESTA EJECUTANDO NADA HAGO LO DE ABAJO
	/*
		if (strcmp(algorithm, "FIFO") == 0) {  // Organizo según el tipo de planificador
			pcb_send = fifo(queue_global_pcb, pcb_received);
		} else if (strcmp(algorithm, "HRRN") == 0) {
			pcb_send = hrrn(logger, queue_global_pcb, pcb_received);
		}
		// mando a cpu, y espera
		// si  de cpu no vuelve que es exit: se vuelve a agregar a la queue

		//SI SE ESTA EJECUTANDO ALGO AGREGAMOS SOLO A LA COLA EL NUEVO PCB
		//queue_push(queue_global_pcb, new_pcb);
		log_info(logger, "El proceso %d se agrego a la cola", pcb_received -> pid);

	*/
}