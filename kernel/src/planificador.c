#include "planificador.h"


pcb* planificador_kernel(t_log* logger,pcb* pcb_recived, char* algorithm,t_queue* queue_global_pcb ){
    log_info(logger,"llego");    
    pcb* pcb_send = malloc(sizeof(pcb));
    if(strcmp(algorithm,"FIFO") == 0){//Organizo segun el tipo de planificador  
       pcb_send = fifo(queue_global_pcb, pcb_recived);
    }else if (strcmp(algorithm,"HRRN") == 0){
        
        pcb_send = hrrn(logger,queue_global_pcb, pcb_recived);
    }   

    return pcb_send;
}