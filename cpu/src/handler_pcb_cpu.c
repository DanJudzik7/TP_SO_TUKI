#include "handler_pcb_cpu.h"

// obtiene la instrucción (lista) actual a ejecutar
void fetch(execution_context* execution_context) {
		print_execution_context(execution_context);
		int sem_value;
		t_instruction* instruction = NULL;
		// ejecuto mientras el flag de desalojo este libre
		do {
			// Esto no consume el semáforo, solo lo consulta
			sem_getvalue(&config_cpu.flag_dislodge, &sem_value);
			instruction = get_instruction(execution_context);
			if (sem_value == 1 && instruction != NULL) {
					decode(execution_context, instruction);
					execution_context->program_counter++;
			}
		} while (sem_value > 0);

		log_warning(config_cpu.logger, "-----------------------------------------------");
		t_package* package_context = serialize_execution_context(execution_context);
		log_info(config_cpu.logger, "Context enviado al Kernel");
		if (!socket_send(config_cpu.connection_kernel, package_context)) {
			log_error(config_cpu.logger, "ERROR AL ENVIAR EL CONTEXT AL KERNEL");
		}

		// Desbloquea el semáforo
		sem_post(&config_cpu.flag_dislodge);

}

// Esta etapa consiste en interpretar qué instrucción es la que se va a ejecutar
// TODO:: y si la misma requiere de una traducción de dirección lógica a dirección física.
execution_context* decode(execution_context* execution_context, t_instruction* instruction) {
	switch (instruction->op_code) {
		case SET:
			execute_set(execution_context, instruction);
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
			break;
		case YIELD:
			log_warning(config_cpu.logger, "Ejecutando un YIELD");
			dislodge();
			break;
		case EXIT:
			log_warning(config_cpu.logger, "Ejecutando un EXIT");
			execute_exit(execution_context);
			dislodge();
			break;
		default:
			break;
	}
	return execution_context;
}

t_instruction* get_instruction(execution_context* ec) {
	if (ec->program_counter >= list_size(ec->instructions->elements)) return NULL;
	return list_get(ec->instructions->elements, ec->program_counter);
}

t_physical_address* MMU(int logic_address, int size, execution_context* ec){
	t_physical_address* physical_address = malloc(sizeof(t_physical_address));
	physical_address->segment = floor(logic_address / config_cpu.max_segment_size);
	physical_address->offset = logic_address % config_cpu.max_segment_size; 
	if (physical_address->offset + size > list_get_by_sid(ec->segment_table, physical_address->segment)->offset)
	{
		log_error(config_cpu.logger, "Segmentation Fault");
		return NULL;
	}
	
	return physical_address;
}

segment* list_get_by_sid(t_list* list, int id) {
	int i = 0;
	while (i < list_size(list)) {
		segment* segment = list_get(list, i);
		if (segment->s_id == id) return segment;
		i++;
	}
	return NULL;
}

void dislodge() {
	log_warning(config_cpu.logger, "Desalojando el Context");
	// Bloquear el semáforo
	sem_wait(&config_cpu.flag_dislodge);
};
