#include "handler_pcb_cpu.h"

// obtiene la instrucción (lista) actual a ejecutar
t_pcb* fetch(t_pcb* t_pcb) {

	int sem_value;
	t_list* instruction = NULL;
	// ejecuto mientras el flag de desalojo este libre
	do{
		printf("Procedemos a ejecutar Instrucciones\n");
		//Esto no consume el semaforo, solo lo consulta
		sem_getvalue(&config_cpu.flag_dislodge, &sem_value);
		printf("\nEl valor obtenido de mi semaforo es: %d ",sem_value);
		instruction = get_instrucction(t_pcb->execution_context);
		if (sem_value > 0) {
            if (instruction != NULL) {
                decode(t_pcb, instruction);
                t_pcb->execution_context->program_counter++;
            } else {
                // No hay más instrucciones
                printf("Error: no existen más instrucciones a ejecutar");
            }
        }
		printf("\nProcediendo a la siguiente ejecucion\n");
	}
	while(sem_value > 0);

	// Desbloquea el semáforo
    	sem_post(&config_cpu.flag_dislodge);

	return t_pcb;
}

// Esta etapa consiste en interpretar qué instrucción es la que se va a ejecutar
// TODO:: y si la misma requiere de una traducción de dirección lógica a dirección física. 
t_pcb* decode(t_pcb* t_pcb, t_list* instruction) {
	log_info(config_cpu.logger, "LLego al decode un PCB con instrucciones a ejecutar\n");
	//To do: esto tiene que eliminarse ya que solamente quiero recibir una lista de instrucciones yo.
	op_code COD_OP = (op_code) list_get(instruction, 0);
	printf("El valor de op_code es: %d\n", (int) COD_OP);
	switch (COD_OP) {
		case SET:
			log_info(config_cpu.logger, "EJECUTANDO UN SET\n");
			execute_set(t_pcb->execution_context, instruction);
			break;
		case MOV_IN:
		case MOV_OUT:
		case I_O:
		case F_OPEN:
		case F_CLOSE:
		case F_SEEK:
		case F_READ:
		case F_WRITE:
		case F_TRUNCATE:
		case WAIT:
		case SIGNAL:
		case CREATE_SEGMENT:
		case DELETE_SEGMENT:
			log_warning(config_cpu.logger, "Implementar función\n");
			break;
		case YIELD:
			dislodge();
			break;
		case EXIT:
			execute_exit(t_pcb->execution_context);
			dislodge();
			break;
		default:
			break;
	}	
	log_info(config_cpu.logger, "LLego al decode un PCB con instrucciones a ejecutar\n");
	return t_pcb;
}

t_list* get_instrucction(execution_context* execution_context){
	return list_get((execution_context->instructions),execution_context->program_counter);
}

void dislodge(){
	log_warning(config_cpu.logger, "Desalojando el pcb\n");
	// Bloquear el semáforo
    sem_wait(&config_cpu.flag_dislodge);
};