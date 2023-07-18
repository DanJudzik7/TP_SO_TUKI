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

t_package* serialize_execution_context(execution_context* ec) {
	t_package* package = package_new(EXECUTION_CONTEXT);
	uint64_t size4 = 4;
	package_nest(package, serialize_instructions(ec->instructions, true));
	package_nest(package, package_new_dict(PROGRAM_COUNTER, &(ec->program_counter), &size4));
	package_nest(package, package_new_dict(UPDATED_STATE, &(ec->updated_state), &size4));
	package_nest(package, serialize_cpu_registers(ec->cpu_register));
	/* segment_table* sg_help = s_malloc(sizeof(segment_table));
	sg_help->pid = ec->pid;
	sg_help->segment_table_pcb = ec->segment_table;
	package_nest(package, serialize_segment_table(ec->segment_table)); */
	if (ec->kernel_request != NULL) package_nest(package, package_new_nested(KERNEL_REQUEST, ec->kernel_request));
	return package;
}

execution_context* deserialize_execution_context(t_package* package) {
	execution_context* ec = s_malloc(sizeof(execution_context));
	uint64_t offset = 0;
	uint64_t offset_start = 0;
	while (package_decode_isset(package, offset)) {
		t_package* nested_package = package_decode(package->buffer, &offset);
		switch (nested_package->type) {
			case EC_INSTRUCTIONS:
				ec->instructions = queue_create();
				deserialize_instructions(nested_package, ec->instructions);
				break;
			case PROGRAM_COUNTER:
				deserialize_program_counter(nested_package->buffer, &(ec->program_counter), &offset_start);
				break;
			case UPDATED_STATE:
				offset_start = 0;
				package_decode_buffer(nested_package->buffer, &(ec->updated_state), &offset_start);
				break;
			case CPU_REGISTERS:
				ec->cpu_register = deserialize_cpu_registers(nested_package->buffer);
				break;
			case SEGMENT_TABLE:
				// ec->segment_table = deserialize_segment_table(nested_package);
				break;
			case KERNEL_REQUEST:
				ec->kernel_request = deserialize_instruction(nested_package);
				break;
			default:
				printf("Error: Tipo de paquete desconocido.\n");
				return NULL;
		}
		package_destroy(nested_package);
	}
	return ec;
}

uint32_t deserialize_program_counter(void* buffer, void* dest, uint64_t* offset) {
	uint64_t* pc_value_64 = s_malloc(sizeof(uint64_t));
	package_decode_buffer(buffer, pc_value_64, offset);
	uint32_t pc_value_32 = (uint32_t)(*pc_value_64);
	free(pc_value_64);
	*(uint32_t*)dest = pc_value_32;
	return pc_value_32;
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

t_package* serialize_cpu_registers(cpu_register* registers) {
	t_package* package = package_new(CPU_REGISTERS);
	uint64_t size4 = 4, size8 = 8, size16 = 16;
	// Registros de 4 bytes
	package_add(package, &(registers->register_4.AX), &size4);
	package_add(package, &(registers->register_4.BX), &size4);
	package_add(package, &(registers->register_4.CX), &size4);
	package_add(package, &(registers->register_4.DX), &size4);
	// Registros de 8 bytes
	package_add(package, &(registers->register_8.EAX), &size8);
	package_add(package, &(registers->register_8.EBX), &size8);
	package_add(package, &(registers->register_8.ECX), &size8);
	package_add(package, &(registers->register_8.EDX), &size8);
	// Registros de 16 bytes
	package_add(package, &(registers->register_16.RAX), &size16);
	package_add(package, &(registers->register_16.RBX), &size16);
	package_add(package, &(registers->register_16.RCX), &size16);
	package_add(package, &(registers->register_16.RDX), &size16);
	return package;
}

cpu_register* deserialize_cpu_registers(void* source) {
	uint64_t offset = 0;
	cpu_register* registers = s_malloc(sizeof(cpu_register));
	// Registros de 4 bytes
	package_decode_buffer(source, registers->register_4.AX, &offset);
	package_decode_buffer(source, registers->register_4.BX, &offset);
	package_decode_buffer(source, registers->register_4.CX, &offset);
	package_decode_buffer(source, registers->register_4.DX, &offset);
	// Registros de 8 bytes
	package_decode_buffer(source, registers->register_8.EAX, &offset);
	package_decode_buffer(source, registers->register_8.EBX, &offset);
	package_decode_buffer(source, registers->register_8.ECX, &offset);
	package_decode_buffer(source, registers->register_8.EDX, &offset);
	// Registros de 16 bytes
	package_decode_buffer(source, registers->register_16.RAX, &offset);
	package_decode_buffer(source, registers->register_16.RBX, &offset);
	package_decode_buffer(source, registers->register_16.RCX, &offset);
	package_decode_buffer(source, registers->register_16.RDX, &offset);
	return registers;
}

void deserialize_segment_table(t_package* package) {
	uint64_t offset = 0;
	while (package_decode_isset(package, offset)) {
		t_package* nested = package_decode(package->buffer, &offset);
		deserialize_segment(nested);
	}
}
void deserialize_segment(t_package* nested) {
	t_instruction* instruction = deserialize_instruction(nested);
	int base = atoi(list_get(instruction->args, 0));
	int offset = atoi(list_get(instruction->args, 1));
	int s_id = atoi(list_get(instruction->args, 2));
	printf("Base: %d\n", base);
	printf("Offset: %d\n", offset);
	printf("S_ID: %d\n", s_id);
}

t_package* serialize_all_segments(t_memory_structure* mem_struct) {
	t_package* package = package_new(COMPACT_FINISHED);
	int j = 0, d = 1;
	t_list* sg;
	while (d < dictionary_size(mem_struct->table_pid_segments)) {
		if (dictionary_has_key(mem_struct->table_pid_segments, string_itoa(j))) {
			sg = dictionary_get(mem_struct->table_pid_segments, string_itoa(j));
			t_package* nested = serialize_segment_table(mem_struct, sg);
			deserialize_segment_table(nested);
			package_nest(package, nested);
			d++;
			j++;
		} else j++;
	}
	return package;
}

t_package* serialize_segment_table(t_memory_structure* mem_struct, t_list* segment_table) {
	t_package* package = package_new(COMPACT_FINISHED);
	for (int i = 0; i < list_size(segment_table); i++) {
		t_package* nested = serialize_segment(list_get(segment_table, i), mem_struct);
		package_nest(package, nested);
	}
	return package;
}

t_package* serialize_segment(segment* segment, t_memory_structure* mem_struct) {
	t_instruction* instruction = instruction_new(SEGMENT);
	list_add(instruction->args, string_itoa(segment->base - mem_struct->heap));
	list_add(instruction->args, string_itoa(segment->offset));
	list_add(instruction->args, string_itoa(segment->s_id));
	t_package* package = serialize_instruction(instruction);
	return package;
}

/* t_package* serialize_segment_read_write(segment_read_write* seg_rw) {
	t_package* package = package_new(F_WRITE_READ);

	uint64_t size = sizeof(uint64_t);
	uint64_t buffer_size = sizeof(seg_rw->buffer);

	package_add(package, &(seg_rw->pid),	&size);
	package_add(package, &(seg_rw->buffer),	&buffer_size);
	package_add(package, &(seg_rw->size),	&size);
	package_add(package, &(seg_rw->offset), &size);
	package_add(package, &(seg_rw->s_id), 	&size);
	return package;
}

segment_read_write* deserialize_segment_read_write(void* source) {
	uint64_t offset = 0;
	segment_read_write* seg_rw = s_malloc(sizeof(segment_read_write));
	memset(seg_rw, 0, sizeof(segment_read_write));

	package_decode_buffer(source, &(seg_rw->pid), &offset);
	package_decode_buffer(source, &(seg_rw->buffer), &offset);
	package_decode_buffer(source, &(seg_rw->size), &offset);
	package_decode_buffer(source, &(seg_rw->offset), &offset);
	package_decode_buffer(source, &(seg_rw->s_id), &offset);

	return seg_rw;
} */

void serialize_package(t_package* package) {
	uint64_t package_size = sizeof(uint64_t) + sizeof(int32_t) + package->size;
	void* stream = s_malloc(package_size);
	uint64_t offset = 0;
	memcpy(stream + offset, &(package->size), sizeof(uint64_t));
	offset += sizeof(uint64_t);
	memcpy(stream + offset, &(package->type), sizeof(int32_t));
	offset += sizeof(int32_t);
	memcpy(stream + offset, package->buffer, package->size);
	package->type = SERIALIZED;
	package->size = package_size;
	free(package->buffer);
	package->buffer = stream;
}