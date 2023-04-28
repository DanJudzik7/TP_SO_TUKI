#include "planificador.h"


pcb* planficador_kernel(pcb* pcb_recived, char* algorithm,t_queue* queue_global_pcb ){

     pcb* pcb_send;

    if(strcmp(algorithm,"FIFO") == 0){//Organizo segun el tipo de planificador 
        
       pcb_send = fifo(pcb_recived);

    }else if (strcmp(algorithm,"HRRN") == 0){
        
        pcb_send = hrrn(pcb_recived);
    }   

    return pcb_send;
}