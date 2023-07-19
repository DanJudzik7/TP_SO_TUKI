#include "handler_cpu.h"

t_instruction* fetch(t_execution_context* ec) {
	if (ec->program_counter >= list_size(ec->instructions->elements)) return NULL;
	return list_get(ec->instructions->elements, ec->program_counter);
}

t_physical_address* decode(t_instruction* instruction, t_execution_context* ec) {
	switch (instruction->op_code) {
		case SET:
			sleep(config_cpu.instruction_delay);
			break;
		case MOV_IN:  // Registro, Dirección Lógica
			return mmu((int)list_get(instruction->args, 1), sizeof(register_pointer(list_get(instruction->args, 0), ec->cpu_register)), ec);
		case MOV_OUT:  // Dirección Lógica, Registro
			return mmu((int)list_get(instruction->args, 0), sizeof(register_pointer(list_get(instruction->args, 1), ec->cpu_register)), ec);
		case F_READ:   // Nombre Archivo, Dirección Lógica, Cantidad de Bytes
		case F_WRITE:  // Nombre Archivo, Dirección Lógica, Cantidad de Bytes
			return mmu((int)list_get(instruction->args, 1), list_get(instruction->args, 2), ec);
	}
	return NULL;
}

bool execute(t_instruction* instruction, t_execution_context* ec, t_physical_address* associated_pa) {
	switch (instruction->op_code) {
		case SET: {
			set_register(list_get(instruction->args, 0), list_get(instruction->args, 1), ec->cpu_register);
			break;
		}
		case MOV_IN: {	// Registro, Dirección Lógica (y associated_pa)
			t_instruction* mem_op = instruction_new(MEM_READ_ADDRESS);
			list_add(mem_op->args, associated_pa->segment);
			list_add(mem_op->args, associated_pa->offset);
			list_add(mem_op->args, sizeof(register_pointer(list_get(instruction->args, 0), ec->cpu_register)));
			list_add(mem_op->args, ec->pid);
			if (!socket_send(config_cpu.socket_memory, serialize_instruction(mem_op))) {
				log_error(config_cpu.logger, "Error al enviar operación a memoria");
				return;
			}
			t_package* package = socket_receive(config_cpu.socket_memory);
			char* value = deserialize_message(package);
			set_register(list_get(instruction->args, 0), value, ec->cpu_register);
			free(value);
			break;
		}
		case MOV_OUT: {	 // Dirección Lógica, Registro (y associated_pa)
			t_instruction* mem_op = instruction_new(MEM_WRITE_ADDRESS);
			list_add(mem_op->args, associated_pa->segment);
			list_add(mem_op->args, associated_pa->offset);
			list_add(mem_op->args, register_pointer(list_get(instruction->args, 1), ec->cpu_register));
			list_add(mem_op->args, ec->pid);
			if (!socket_send(config_cpu.socket_memory, serialize_instruction(mem_op))) {
				log_error(config_cpu.logger, "Error al enviar operación a memoria");
				return;
			}
			t_package* package = socket_receive(config_cpu.socket_memory);
			if (package->type == SEG_FAULT)
				log_error(config_cpu.logger, "Error al escribir en memoria");
			else if (package->type == OK_INSTRUCTION)
				log_info(config_cpu.logger, "Escritura en memoria exitosa");
			break;
		}
		case F_READ:  // filename, logical address, bytes count
		case F_WRITE: {
			ec->kernel_request = instruction_duplicate(instruction);
			list_add(ec->kernel_request->args, associated_pa->segment);
			list_add(ec->kernel_request->args, associated_pa->offset);
		}
		case YIELD:
			log_warning(config_cpu.logger, "Desalojando proceso");
			return true;
		case I_O:
		case F_OPEN:
		case F_CLOSE:
		case F_SEEK:
		case F_TRUNCATE:
		case WAIT:
		case SIGNAL:
		case CREATE_SEGMENT:
		case DELETE_SEGMENT:
		case EXIT:
			log_info(config_cpu.logger, "Ejecutando instrucción %d y desalojando", instruction->op_code);
			ec->kernel_request = instruction;
			return true;
		default:
			log_error(config_cpu.logger, "Error: Operación inválida");
			break;
	}
	return false;
}

void set_register(char* register_name, char* value, cpu_register* registers) {
	if (value == NULL) {
		log_error(config_cpu.logger, "Error: Valor inválido o vacío");
		return;
	}
	char* register_ptr = register_pointer(register_name, registers);
	if (register_ptr == NULL) {
		log_error(config_cpu.logger, "Error: Registro %s no válido", register_name);
		return;
	}
	size_t register_size = sizeof(register_ptr);
	size_t value_len = strlen(value);
	if (value_len > register_size) {
		log_error(config_cpu.logger, "Error: Tamaño del registro -> %lu", value_len);
		log_error(config_cpu.logger, "Error: Tamaño del value_len -> %lu", register_size);
		log_error(config_cpu.logger, "Error: El valor excede el tamaño del registro");
		return;
	}
	strncpy(register_ptr, value, register_size - 1);
	((char*)register_ptr)[register_size - 1] = '\0';
	log_info(config_cpu.logger, "Asignando en %s: %s", register_name, value);
	free(register_ptr);
}