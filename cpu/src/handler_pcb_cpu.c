#include "handler_pcb_cpu.h"

// obtiene la instrucción (lista) actual a ejecutar
void fetch(execution_context* execution_context) {
		printf("llego\n");
		int sem_value;
		t_instruction* instruction = NULL;
		// ejecuto mientras el flag de desalojo este libre
		do {
			printf("Procedemos a ejecutar Instrucciones\n");
			// Esto no consume el semáforo, solo lo consulta
			sem_getvalue(&config_cpu.flag_dislodge, &sem_value);
			printf("El valor obtenido de mi semáforo es: %d\n", sem_value);
			instruction = get_instruction(execution_context);
			if (sem_value == 1) {
				if (instruction != NULL) {
					decode(execution_context, instruction);
					execution_context->program_counter++;
				} else {
					// No hay más instrucciones
					printf("Error: no existen más instrucciones a ejecutar\n");
				}
			}
			printf("Procediendo a la siguiente ejecuci+on\n");
		} while (sem_value > 0);

		// Desbloquea el semáforo
		sem_post(&config_cpu.flag_dislodge);

		// sem_post(&config_cpu.flag_dislodge);   Desbloquear el estado running una vez implementado el hilo
		// t_package* package = package_create(MESSAGE_OK);
		// package_add(package, execution_context , sizeof(*execution_context));
		// socket_send_package(package,args->kernel_socket);
}

// Esta etapa consiste en interpretar qué instrucción es la que se va a ejecutar
// TODO:: y si la misma requiere de una traducción de dirección lógica a dirección física.
execution_context* decode(execution_context* execution_context, t_instruction* instruction) {
	log_info(config_cpu.logger, "LLegó al decode un PCB con instrucciones a ejecutar\n");
	// To do: esto tiene que eliminarse ya que solamente quiero recibir una lista de instrucciones yo.
	printf("El valor de op code es: %d\n", instruction->op_code);
	switch (instruction->op_code) {
		case SET:
			log_info(config_cpu.logger, "EJECUTANDO UN SET\n");
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
			log_warning(config_cpu.logger, "Implementar función\n");
			break;
		case YIELD:
			dislodge();
			break;
		case EXIT:
			execute_exit(execution_context);
			dislodge();
			break;
		default:
			break;
	}
	log_info(config_cpu.logger, "LLego al decode un PCB con instrucciones a ejecutar\n");
	return execution_context;
}

t_instruction* get_instruction(execution_context* ec) {
	if (ec->program_counter >= list_size(ec->instructions->elements)) return NULL;
	return list_get(ec->instructions->elements, ec->program_counter);
}

void dislodge() {
	log_warning(config_cpu.logger, "Desalojando el pcb");
	// Bloquear el semáforo
	sem_wait(&config_cpu.flag_dislodge);
};
