#include "handler_cpu.h"

t_instruction* fetch(t_execution_context* ec) {
	if (ec->program_counter >= list_size(ec->instructions->elements)) return NULL;
	return list_get(ec->instructions->elements, ec->program_counter);
}

t_physical_address* decode(t_instruction* instruction, t_execution_context* ec) {
	char* args = string_new();
	for (int i = 0; i < list_size(instruction->args); i++) string_append_with_format(&args, "%s, ", (char*)list_get(instruction->args, i));
	log_warning(config_cpu.logger, "PID: %d - Ejecutando: %s - %s", ec->pid, read_op_code(instruction->op_code), args);
	free(args);
	switch (instruction->op_code) {
		case SET:
			usleep(config_cpu.instruction_delay);
			break;
		case MOV_IN: {	// Registro, Dirección Lógica
			char* register_name = list_get(instruction->args, 0);
			char* logical_address = list_get(instruction->args, 1);
			return mmu(atoi(logical_address), size_of_register_pointer(register_name, ec->cpu_register), ec);
		}
		case MOV_OUT: {	 // Dirección Lógica, Registro
			char* logical_address = list_get(instruction->args, 0);
			char* register_name = list_get(instruction->args, 1);
			return mmu(atoi(logical_address), size_of_register_pointer(register_name, ec->cpu_register), ec);
		}
		case F_READ:	 // Nombre Archivo, Dirección Lógica, Cantidad de Bytes
		case F_WRITE: {	 // Nombre Archivo, Dirección Lógica, Cantidad de Bytes
			char* logical_address = list_get(instruction->args, 1);
			char* bytes_count = list_get(instruction->args, 2);
			return mmu(atoi(logical_address), atoi(bytes_count), ec);
		}
	}
	return NULL;
}

void execute(t_instruction* instruction, t_execution_context* ec, t_physical_address* associated_pa) {
	switch (instruction->op_code) {
		case SET: {
			set_register(list_get(instruction->args, 0), list_get(instruction->args, 1), ec->cpu_register);
			break;
		}
		case MOV_IN: {	// Registro, Dirección Lógica (y associated_pa)
			if (associated_pa == NULL) break;
			t_instruction* mem_op = instruction_new(MEM_READ_ADDRESS);
			list_add(mem_op->args, string_itoa(associated_pa->segment));
			list_add(mem_op->args, string_itoa(associated_pa->offset));
			list_add(mem_op->args, string_itoa(size_of_register_pointer(list_get(instruction->args, 0), ec->cpu_register)));
			list_add(mem_op->args, string_itoa(ec->pid));
			if (!socket_send(config_cpu.socket_memory, serialize_instruction(mem_op))) {
				log_error(config_cpu.logger, "Error al enviar operación a memoria");
				break;
			}
			t_package* package = socket_receive(config_cpu.socket_memory);
			if (package == NULL || package->type != MESSAGE_OK) {
				log_error(config_cpu.logger, "Error al leer de memoria");
				break;
			}
			char* value = deserialize_message(package);
			set_register(list_get(instruction->args, 0), value, ec->cpu_register);
			log_warning(config_cpu.logger, "PID: %d - Acción: LEER - Segmento: %d - Dirección Física: %d - Valor: %s", ec->pid, associated_pa->segment, associated_pa->offset, value);
			free(value);
			break;
		}
		case MOV_OUT: {	 // Dirección Lógica, Registro (y associated_pa)
			if (associated_pa == NULL) break;
			t_instruction* mem_op = instruction_new(MEM_WRITE_ADDRESS);
			list_add(mem_op->args, string_itoa(associated_pa->segment));
			list_add(mem_op->args, string_itoa(associated_pa->offset));
			char* value = register_pointer(list_get(instruction->args, 1), ec->cpu_register);
			list_add(mem_op->args, value);
			list_add(mem_op->args, string_itoa(ec->pid));
			if (!socket_send(config_cpu.socket_memory, serialize_instruction(mem_op))) {
				log_error(config_cpu.logger, "Error al enviar operación a memoria");
				break;
			}
			t_package* package = socket_receive(config_cpu.socket_memory);
			if (package == NULL) {
				log_error(config_cpu.logger, "Error al escribir en memoria");
				break;
			}
			if (package->type == SEG_FAULT) {
				log_error(config_cpu.logger, "Error en memoria: Segmentation Fault");
				ec->kernel_request = instruction_new(EXIT);
				list_add(ec->kernel_request->args, string_itoa(9));
				break;
			} else if (package->type != OK_INSTRUCTION) {
				log_error(config_cpu.logger, "Error desconocido en memoria");
				break;
			}
			log_warning(config_cpu.logger, "PID: %d - Acción: ESCRIBIR - Segmento: %d - Dirección Física: %d - Valor: %s", ec->pid, associated_pa->segment, associated_pa->offset, value);
			break;
		}
		case F_READ:  // filename, logical address, bytes count
		case F_WRITE: {
			if (associated_pa == NULL) {
				log_error(config_cpu.logger, "Error: Dirección lógica inválida");
				break;
			}
			ec->kernel_request = instruction_duplicate(instruction);
			list_add(ec->kernel_request->args, string_itoa(associated_pa->segment));
			list_add(ec->kernel_request->args, string_itoa(associated_pa->offset));
			break;
		}
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
		case YIELD:
			ec->kernel_request = instruction_duplicate(instruction);
			break;
		default:
			log_error(config_cpu.logger, "Se ha recibido una operación inválida");
			break;
	}
}

void set_register(char* register_name, char* value, t_registers* registers) {
	if (value == NULL) {
		log_error(config_cpu.logger, "Error: Valor inválido o vacío");
		return;
	}
	char* register_ptr = register_pointer(register_name, registers);
	if (register_ptr == NULL) {
		log_error(config_cpu.logger, "Error: Registro %s inválido", register_name);
		return;
	}
	size_t register_size = size_of_register_pointer(register_name, registers);
	size_t value_len = strlen(value);
	if (value_len > register_size) {
		log_error(config_cpu.logger, "Error: El valor (longitud %lu) excede el tamaño del registro (longitud %lu)", value_len, register_size);
		return;
	}
	strncpy(register_ptr, value, register_size);
	log_info(config_cpu.logger, "Execute: Se asignó en %s: %s", register_name, value);
}