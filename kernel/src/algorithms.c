#include "algorithms.h"

pcb* fifo(t_queue* queue_global_pcb, pcb *new_pcb){
  pcb* next_pcb = malloc(sizeof(pcb));
  queue_push(queue_global_pcb, new_pcb); //Agrego el nuevo pcb al final de la cola
  next_pcb = queue_peek(queue_global_pcb); //Devuelve el primer elemento de la cola sin extraerlo
  queue_pop(queue_global_pcb); //quita el primer elemento de la cola

  return next_pcb;
}

pcb* hrrn(t_log* logger,t_queue* queue_global_pcb, pcb *new_pcb){  

    // Se agrega el nuevo PCB a la cola global
    queue_push(queue_global_pcb, new_pcb);
    
    // Se obtiene la hora actual del sistema
    time_t current_time = time(NULL);
    
    // Se inicializa la cola temporal
    t_queue* queue_temporal = queue_create();
    
    // Se inicializa el valor del response ratio más alto encontrado
    float highest_response_ratio = 0;
    
    // Se inicializa el PCB siguiente como nulo
    pcb* next_pcb = malloc(sizeof(pcb));
    next_pcb = NULL; 
    
    // Se inicializa el PCB actual como nulo
    pcb* p = malloc(sizeof(pcb));
    
    // Se itera sobre la cola global de PCBs
    while (!queue_is_empty(queue_global_pcb)) {
        // Se obtiene el primer PCB de la cola
        p = queue_peek(queue_global_pcb);
        
        // Se calcula el response ratio del proceso actual
        float response_ratio = (float)(current_time - p->last_ready_time + p->aprox_burst_time) / (float)p->aprox_burst_time;
        
        // Si el response ratio del proceso actual es mayor que el response ratio más alto encontrado
        if (response_ratio > highest_response_ratio) {
            // Se actualiza el response ratio más alto encontrado
            highest_response_ratio = response_ratio;
            
            // Si el PCB siguiente no es nulo, se agrega a la cola temporal
            if (next_pcb != NULL) {
              queue_push(queue_temporal, next_pcb);
            }
            
            // El siguiente PCB es el proceso actual
            next_pcb = p;
        } else {
            // Si el response ratio del proceso actual no es mayor que el response ratio más alto encontrado, se agrega a la cola temporal
            queue_push(queue_temporal, p);
        }
        
        // Se elimina el proceso actual de la cola global
        queue_pop(queue_global_pcb);
        
       
        
    }
    // Se libera la memoria del proceso actual
    free(p);
    // Se limpia la cola global de PCBs
    queue_clean(queue_global_pcb);
    
    // La cola global de PCBs ahora es la cola temporal
    queue_global_pcb = queue_temporal;
    
    // Se libera la memoria de la cola temporal
    free(queue_temporal);
    
    // Se actualiza el last_ready_time del proceso seleccionado
    next_pcb->last_ready_time = current_time;

    // Se devuelve el proceso con el mayor response ratio
    return next_pcb;
}