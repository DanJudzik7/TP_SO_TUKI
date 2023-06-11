#include "handler_pcb_cpu.h"

// obtiene la instrucción (lista) actual a ejecutar
void fetch(execution_context* execution_context) {
		printf("llego\n");
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
			printf("Procediendo a la siguiente instruccion\n");
		} while (sem_value > 0);
		
		t_package* package_context = serialize_execution_context(execution_context);
		log_info(config_cpu.logger, "Context enviado al Kernel");
		if (!socket_send(config_cpu.connection_kernel, package_context)) {
			log_error(config_cpu.logger, "ERROR AL ENVIAR EL CONTEXT AL KERNEL");
		}

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
	// TODO: esto tiene que eliminarse ya que solamente quiero recibir una lista de instrucciones yo.
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

void dislodge() {
	log_warning(config_cpu.logger, "Desalojando el Context");
	// Bloquear el semáforo
	sem_wait(&config_cpu.flag_dislodge);
};
