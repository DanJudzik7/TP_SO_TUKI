#include "shared_serializer.h"

t_package* serialize_message(char* value, bool is_flaw) {
	t_package* package = package_new(is_flaw ? MESSAGE_FLAW : MESSAGE_OK);
	package_write(package, value);
	return package;
}

char* deserialize_message(t_package* package) {
	uint64_t offset = 0;
	char* message = package_decode_string(package->buffer, &offset);
	package_destroy(package);
	return message;
}

t_package* serialize_execution_context(t_execution_context* ec) {
	t_package* package = package_new(EXECUTION_CONTEXT);
	uint64_t size4 = 4;
	package_nest(package, serialize_instructions(ec->instructions, true));
	package_nest(package, package_new_dict(PROGRAM_COUNTER, &(ec->program_counter), &size4));
	package_nest(package, package_new_dict(PROCESS_PID, &(ec->pid), &size4));
	package_nest(package, serialize_cpu_registers(ec->cpu_register));
	package_nest(package, serialize_segments_table(ec->segments_table, SEGMENTS_TABLE, 0));
	if (ec->kernel_request != NULL) package_nest(package, package_new_nested(KERNEL_REQUEST, serialize_instruction(ec->kernel_request)));
	return package;
}

t_execution_context* deserialize_execution_context(t_package* package) {
	t_execution_context* ec = s_malloc(sizeof(t_execution_context));
	ec->kernel_request = NULL;
	uint64_t offset = 0;
	while (package_decode_isset(package, offset)) {
		t_package* nested_package = package_decode(package->buffer, &offset);
		switch (nested_package->type) {
			case EC_INSTRUCTIONS:
				ec->instructions = queue_create();
				deserialize_instructions(nested_package, ec->instructions);
				break;
			case PROGRAM_COUNTER:
				package_decode_buffer(nested_package->buffer, &(ec->program_counter), NULL);
				break;
			case PROCESS_PID:
				package_decode_buffer(nested_package->buffer, &(ec->pid), NULL);
				break;
			case CPU_REGISTERS:
				ec->cpu_register = deserialize_cpu_registers(nested_package->buffer);
				break;
			case SEGMENTS_TABLE:
				ec->segments_table = deserialize_segment_table(nested_package);
				break;
			case KERNEL_REQUEST: {
				t_package* kernel_request = package_decode(nested_package->buffer, NULL);
				ec->kernel_request = deserialize_instruction(kernel_request);
				package_destroy(kernel_request);
				break;
			}
			default:
				printf("Error: Tipo de paquete desconocido.\n");
				return NULL;
		}
		package_destroy(nested_package);
	}
	return ec;
}

t_package* serialize_instructions(t_queue* instructions, bool is_ec) {
	t_package* package = package_new(is_ec ? EC_INSTRUCTIONS : INSTRUCTIONS);
	// Recorre la cola de instrucciones y las serializa
	for (int i = 0; i < queue_size(instructions); i++)
		package_nest(package, serialize_instruction(list_get(instructions->elements, i)));
	return package;
}

t_package* serialize_instruction(t_instruction* instruction) {
	t_package* package = package_new(instruction->op_code);
	for (int j = 0; j < list_size(instruction->args); j++) package_write(package, list_get(instruction->args, j));
	return package;
}

void deserialize_instructions(t_package* package, t_queue* instructions) {
	uint64_t offset = 0;
	// Va buscando todas las instrucciones
	while (package_decode_isset(package, offset)) {
		t_package* instruction_package = package_decode(package->buffer, &offset);
		queue_push(instructions, deserialize_instruction(instruction_package));
		package_destroy(instruction_package);
	}
}

t_instruction* deserialize_instruction(t_package* package) {
	t_instruction* instruction = instruction_new(package->type);
	uint64_t offset_list = 0;
	// Carga los args de package_add
	while (package_decode_isset(package, offset_list)) list_add(instruction->args, package_decode_string(package->buffer, &offset_list));
	return instruction;
}

t_package* serialize_cpu_registers(t_registers* registers) {
	t_package* package = package_new(CPU_REGISTERS);
	uint64_t size4 = 4, size8 = 8, size16 = 16;
	// Registros de 4 bytes
	package_add(package, registers->AX, &size4);
	package_add(package, registers->BX, &size4);
	package_add(package, registers->CX, &size4);
	package_add(package, registers->DX, &size4);
	// Registros de 8 bytes
	package_add(package, registers->EAX, &size8);
	package_add(package, registers->EBX, &size8);
	package_add(package, registers->ECX, &size8);
	package_add(package, registers->EDX, &size8);
	// Registros de 16 bytes
	package_add(package, registers->RAX, &size16);
	package_add(package, registers->RBX, &size16);
	package_add(package, registers->RCX, &size16);
	package_add(package, registers->RDX, &size16);
	return package;
}

t_registers* deserialize_cpu_registers(void* source) {
	uint64_t offset = 0;
	t_registers* registers = s_malloc(sizeof(t_registers));
	// Registros de 4 bytes
	package_decode_buffer(source, registers->AX, &offset);
	package_decode_buffer(source, registers->BX, &offset);
	package_decode_buffer(source, registers->CX, &offset);
	package_decode_buffer(source, registers->DX, &offset);
	// Registros de 8 bytes
	package_decode_buffer(source, registers->EAX, &offset);
	package_decode_buffer(source, registers->EBX, &offset);
	package_decode_buffer(source, registers->ECX, &offset);
	package_decode_buffer(source, registers->EDX, &offset);
	// Registros de 16 bytes
	package_decode_buffer(source, registers->RAX, &offset);
	package_decode_buffer(source, registers->RBX, &offset);
	package_decode_buffer(source, registers->RCX, &offset);
	package_decode_buffer(source, registers->RDX, &offset);
	return registers;
}

t_list* deserialize_segment_table(t_package* package) {
	uint64_t offset = 0;
	t_list* segment_table = list_create();
	while (package_decode_isset(package, offset)) {
		t_package* nested = package_decode(package->buffer, &offset);
		t_segment* segment = deserialize_segment(nested);
		list_add(segment_table, segment);
	}
	return segment_table;
}

t_segment* deserialize_segment(t_package* nested) {
	t_instruction* instruction = deserialize_instruction(nested);
	t_segment* segment = s_malloc(sizeof(segment));
	int base_value = atoi(list_get(instruction->args, 0));
	segment->base = malloc(sizeof(int));
	memcpy(segment->base, &base_value, sizeof(int));
	segment->offset = atoi(list_get(instruction->args, 1));
	segment->s_id = atoi(list_get(instruction->args, 2));
	instruction_destroy(instruction);
	return segment;
}

t_package* serialize_all_segments_tables(t_memory_structure* mem_struct) {
	t_package* package = package_new(ALL_SEGMENTS_TABLES);
	int pid = 0, count = 1;
	t_list* sg;
	while (count < dictionary_size(mem_struct->table_pid_segments)) {
		if (dictionary_has_key(mem_struct->table_pid_segments, string_itoa(pid))) {
			sg = dictionary_get(mem_struct->table_pid_segments, string_itoa(pid));
			t_package* nested = serialize_segments_table(sg, pid, mem_struct->heap);
			package_nest(package, nested);
			count++;
		}
		pid++;
	}
	return package;
}

t_package* serialize_segments_table(t_list* segment_table, uint32_t type, void* heap_pointer) {
	t_package* package = package_new(type);
	for (int i = 0; i < list_size(segment_table); i++) {
		t_package* nested = serialize_segment(list_get(segment_table, i), heap_pointer);
		package_nest(package, nested);
	}
	return package;
}

t_package* serialize_segment(t_segment* segment, void* heap_pointer) {
	t_instruction* instruction = instruction_new(SEGMENT);
	list_add(instruction->args, string_itoa(segment->base - heap_pointer));
	list_add(instruction->args, string_itoa(segment->offset));
	list_add(instruction->args, string_itoa(segment->s_id));
	t_package* package = serialize_instruction(instruction);
	return package;
}

t_dictionary* deserialize_all_segments_tables(t_package* package) {
	t_dictionary* table_pid_segments = dictionary_create();
	uint64_t offset = 0;
	while (package_decode_isset(package, offset)) {
		t_package* nested = package_decode(package->buffer, &offset);
		t_list* segment_table = deserialize_segment_table(nested);
		dictionary_put(table_pid_segments, string_itoa(nested->type), segment_table);
	}
	return table_pid_segments;
}