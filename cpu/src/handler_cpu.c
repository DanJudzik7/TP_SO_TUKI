#include "handler_cpu.h"

t_instruction* fetch(execution_context* ec) {
	if (ec->program_counter >= list_size(ec->instructions->elements)) return NULL;
	return list_get(ec->instructions->elements, ec->program_counter);
}

void decode(t_instruction* instruction, execution_context* ec) {
	switch (instruction->op_code) {
		case SET:
			sleep(config_cpu.instruction_delay);
			break;
		case MOV_IN: // Registro, Dirección Lógica
			list_replace(instruction->args, 1, mmu(list_get(instruction->args, 1), sizeof(register_pointer(list_get(instruction->args, 0), ec->cpu_register)), ec));
			break;
		case MOV_OUT: // Dirección Lógica, Registro
			list_replace(instruction->args, 0, mmu(list_get(instruction->args, 0), sizeof(register_pointer(list_get(instruction->args, 1), ec->cpu_register)), ec));
			break;
		case F_READ: // Nombre Archivo, Dirección Lógica, Cantidad de Bytes
		case F_WRITE: // Nombre Archivo, Dirección Lógica, Cantidad de Bytes
			list_replace(instruction->args, 1, mmu(list_get(instruction->args, 1), list_get(instruction->args, 2), ec));
			break;
	}
}

void execute(t_instruction* instruction, execution_context* ec) {
	switch (instruction->op_code) {
		case SET:
			execute_set(ec, instruction);
			break;
		case MOV_IN: { // Registro, t_physical_address {segment, offset}
			t_physical_address* pa = list_get(instruction->args, 1);
			t_instruction* mem_op = malloc(sizeof(t_instruction));
			mem_op->op_code = MEM_READ_ADDRESS;
			mem_op->args = list_create();
			list_add(mem_op->args, pa->segment);
			list_add(mem_op->args, pa->offset);
			list_add(mem_op->args, sizeof(register_pointer(list_get(instruction->args, 0), ec->cpu_register)));
			list_add(mem_op->args, ec->pid);
			if (!socket_send(config_cpu.socket_memory, serialize_instruction(mem_op))) {
				log_error(config_cpu.logger, "Error al enviar operación al memoria");
			}
			t_package* package_receive_memory = socket_receive(config_cpu.socket_memory);
			char* str_write = deserialize_message(package_receive_memory);
		}
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
			execute_exit(ec);
			dislodge();
			break;
		default:
			break;
	}
	return ec;
}

void execute_set(execution_context* execution_context, t_instruction* instruction) {
	// Lo paso a un int con atoi el valor proporcionado en la lista -> APLICA?
	char* value = list_get(instruction->args, 1);
    if (value == NULL) {
		log_info(config_cpu.logger, "Error: Valor inválido o vacío");
		return;
	}

	char* name = list_get(instruction->args, 0);
	char* register_ptr = register_pointer(name, execution_context->cpu_register);
    if (register_ptr == NULL) {
        log_info(config_cpu.logger, "Error: Registro %s no válido", name);
        return;
    }

	// Comparo el registro y asigno el valor correspondiente
	log_info(config_cpu.logger, "Asignando en %s: %s", name, value);
	size_t register_size = sizeof(register_ptr);
	size_t value_len = strlen(value);
	if (value_len > register_size) {
		log_info(config_cpu.logger, "Error: Tamaño del registro -> %lu", value_len);
		log_info(config_cpu.logger, "Error: Tamaño del value_len -> %lu", register_size);
		log_info(config_cpu.logger, "Error: El valor excede el tamaño del registro");
		return;
	}
	strncpy(register_ptr, value, register_size - 1);
	((char*)register_ptr)[register_size - 1] = '\0';
}

void execute_exit(execution_context* execution_context) {
	execution_context->updated_state = EXIT_PROCESS;
}


void dislodge() {
	log_warning(config_cpu.logger, "Desalojando el Context");
	// Bloquear el semáforo
	sem_wait(&config_cpu.flag_dislodge);
};
