#include "handler_state.h"

void state_exit_process(t_pcb* pcb, t_global_config_kernel *gck) {
    log_warning(gck->logger, "----------------------El PCB %d tiene estado exit----------------------", pcb->pid);
	//TODO: HARDCODEADO hasta que se pueda mover a long_term_shedule
	log_info(gck->logger, "Se removió un proceso terminado");
	if(!socket_send(pcb->pid, package_new(MESSAGE_PCB_FINISHED))) log_error(gck->logger, "Error al informar finalizacion a la consola %d",pcb->pid);
	socket_close(pcb->pid);
	pcb_destroy(pcb);
	gck->max_multiprogramming += 1;
	long_term_schedule(gck);
	sem_post(&gck->flag_with_pcb);
}

void state_signal_or_wait(t_pcb* pcb, t_global_config_kernel *gck){
       resources_handler(pcb, pcb->state, gck);
}

void state_yield(t_pcb* pcb, t_global_config_kernel *gck){
	if(gck->algorithm_is_hrrn) {
		//TODO: calcular un nuevo estimado para su próxima ráfaga utilizando la fórmula de promedio ponderado vista en clases.
		//if(gck->algorithm_is_hrrn) 
		//pcb->aprox_burst_time =
	}
       queue_push(gck->active_pcbs, pcb);
}