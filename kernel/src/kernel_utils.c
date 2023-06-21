#include "kernel_utils.h"

bool no_more_instructions(execution_context* ec){
	return ec->program_counter>= queue_size(ec->instructions);
}

void resources_handler(t_pcb* pcb, process_state process_state, t_global_config_kernel* gck){

	char* resource_request = get_resource_name(pcb);
	printf("El recurso requerido es: %s", resource_request);
	if(!dictionary_has_key(gck->resources,resource_request)){
		printf("El recurso existe");
		pcb->state = EXIT_PROCESS;
		return;
	}
	resources_table* resource = (resources_table* ) dictionary_get(gck->resources, resource_request);
	if(process_state == WAIT){
		if(resource->instances >= 0) {
			(resource->instances)--;
			printf("Las instancias del recurso quedaro reducidas a -> %i", resource->instances);
		}
		// SI EL PROCESO ES MENOR A 0 ESTRICTAMENTE, LO MANDO A LA COLA DE BLOQUEADOS DE ESE RECURSO
		// TODO: NO DICE NADA DE BLOQUEAR, CONSULTAR si pcb->state = BLOCK; aunque da igual.
		else queue_push(resource->resource_queue, pcb);
	}
	if(process_state == SIGNAL){
		if(resource->instances > 0) {
			(resource->instances)++;
			printf("Las instancias del recurso aumentaron a -> %i", resource->instances);
			// se devuelve la ejecución al proceso que peticionó el SIGNAL.
			t_pcb* pcb_priority = queue_pop(resource->resource_queue);
			gck->pcb_priority_helper = pcb_priority;
		}
		// TODO: NO DICE NADA DE ESTE SIGNAL, AVERIGUAR ? igual dudo que haga algo
	}
}

char* get_resource_name(t_pcb* pcb){
	//Obtengo la instruccion apuntada por el program counter actual
	t_instruction* instruction =  list_get(pcb->execution_context->instructions->elements, pcb->execution_context->program_counter);
	//Obtengo el nombre del recurso
	return list_get(instruction->args, 1);
}