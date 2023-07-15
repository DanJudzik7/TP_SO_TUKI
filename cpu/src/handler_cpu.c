#include "handler_cpu.h"

void decode(t_instruction* instruction, execution_context* ec) {
	switch (instruction->op_code) {
		case SET:
			sleep(config_cpu.instruction_delay);
			break;
		case MOV_IN:  // Registro, Dirección Lógica
			list_replace(instruction->args, 1, mmu((int)list_get(instruction->args, 1), sizeof(register_pointer(list_get(instruction->args, 0), ec->cpu_register)), ec));
			break;
		case MOV_OUT:  // Dirección Lógica, Registro
			list_replace(instruction->args, 0, mmu((int)list_get(instruction->args, 0), sizeof(register_pointer(list_get(instruction->args, 1), ec->cpu_register)), ec));
			break;
		case F_READ:   // Nombre Archivo, Dirección Lógica, Cantidad de Bytes
		case F_WRITE:  // Nombre Archivo, Dirección Lógica, Cantidad de Bytes
			list_replace(instruction->args, 1, mmu((int)list_get(instruction->args, 1), list_get(instruction->args, 2), ec));
			break;
	}
}

void execute(t_instruction* instruction, execution_context* ec) {
	switch (instruction->op_code) {
		case SET: {
			set_register(list_get(instruction->args, 0), list_get(instruction->args, 1), ec->cpu_register);
			break;
		}
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
				log_error(config_cpu.logger, "Error al enviar operación a memoria");
				return;
			}
			t_package* package = socket_receive(config_cpu.socket_memory);
			char* value = deserialize_message(package);
			set_register(list_get(instruction->args, 0), value, ec->cpu_register);
			free(value);
			break;
		}
		case MOV_OUT: { // t_physical_address {segment, offset}, Registro
			t_physical_address* pa = list_get(instruction->args, 0);
			t_instruction* mem_op = malloc(sizeof(t_instruction));
			mem_op->op_code = MEM_WRITE_ADDRESS;
			mem_op->args = list_create();
			list_add(mem_op->args, pa->segment);
			list_add(mem_op->args, pa->offset);
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
		case I_O: { // Tiempo
			log_error(config_cpu.logger, "Instrucción IO: Manejo de recursos no implementado");
		}
		case F_OPEN: {
			
		}
		case F_CLOSE:
		case F_SEEK:
		case F_READ: { // NAME(0) posicion(1) tamaño_leer(2) PID(3) S_ID(4) OFFSET(5)
			
		}
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

void execute_exit(execution_context* execution_context) {
	execution_context->updated_state = EXIT_PROCESS;
}

void dislodge() {
	log_warning(config_cpu.logger, "Desalojando el Context");
	// Bloquear el semáforo
	sem_wait(&config_cpu.flag_dislodge);
};
