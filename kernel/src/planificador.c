#include "planificador.h"


pcb* planficador_kernel(pcb new_pcb, char* algorithm,t_queue* queue_global_pcb ){
    pcb* pcb_to_send = malloc(sizeof(pcb));
    if(strcmp(algorithm,"FIFO") == 0){//Organizo segun el tipo de planificador 

    }else{

    }
    return pcb_to_send;
}