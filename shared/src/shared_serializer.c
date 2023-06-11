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
    uint64_t program_counter_64 = ec->program_counter;
	package_nest(package, package_new_dict(PROGRAM_COUNTER, &(program_counter_64), &size4));
	package_nest(package, package_new_dict(UPDATED_STATE, &(ec->updated_state), &size4));
	package_nest(package, serialize_cpu_registers(ec->cpu_register));
	package_nest(package, serialize_segment_table(ec->segment_table));

	printf("\nTamaño TOTAL serializado del paquete: %lu bytes\n", package->size);
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
				ec->program_counter = deserialize_program_counter(nested_package->buffer);
				break;
			case UPDATED_STATE:
				package_decode_buffer(nested_package->buffer, &(ec->updated_state), &offset_start);
				break;
			case CPU_REGISTERS:
				ec->cpu_register = deserialize_cpu_registers(nested_package->buffer);
				break;
			case SEGMENT_TABLE:
				ec->segment_table = deserialize_segment_table(nested_package->buffer);
				break;
			default:
				printf("Error: Tipo de paquete desconocido.\n");
				return NULL;
		}
		package_destroy(nested_package);
	}
	printf("\nTamaño deserealizado TOTAL -> %lu bytes\n", offset);
	return ec;
}

uint32_t deserialize_program_counter(void* buffer){
	 uint64_t* program_counter_64 = s_malloc(sizeof(uint64_t));
	memcpy(program_counter_64, buffer, sizeof(uint64_t));
    uint32_t program_counter_32 = (uint32_t)(*program_counter_64);
    free(program_counter_64);
    return program_counter_32;
}

t_package* serialize_instructions(t_queue* instructions, bool is_ec) {
	t_package* package = package_new(is_ec ? EC_INSTRUCTIONS : INSTRUCTIONS);
	// Recorre la cola de instrucciones y las serializa
	for (int i = 0; i < queue_size(instructions); i++) {
		t_instruction* instruction = list_get(instructions->elements, i);
		t_package* nested = package_new(instruction->op_code);
		// Serializa los args de la instrucción
		for (int j = 0; j < list_size(instruction->args); j++) package_write(nested, list_get(instruction->args, j));
		package_nest(package, nested);
	}
	return package;
}

void deserialize_instructions(t_package* package, t_queue* instructions) {
	uint64_t offset = 0;
	// Va buscando todas las instrucciones
	while (package_decode_isset(package, offset)) {
		t_package* instruction_package = package_decode(package->buffer, &offset);
		t_instruction* instruction = s_malloc(sizeof(t_instruction));
		instruction->op_code = instruction_package->type;
		instruction->args = list_create();
		uint64_t offset_list = 0;
		// Carga los args de package_add
		while (package_decode_isset(instruction_package, offset_list)) list_add(instruction->args, package_decode_string(instruction_package->buffer, &offset_list));
		queue_push(instructions, instruction);
		package_destroy(instruction_package);
	}
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

t_package* serialize_segment_table(segment_table* st) {
	t_package* package = package_new(SEGMENT_TABLE);
	// Implementar segment tables acá. Por ahora, manda solo un dato de ejemplo.
	uint64_t size = sizeof(uint32_t);
	package_add(package, &(st->id), &size);
	return package;
}

segment_table* deserialize_segment_table(void* source) {
	uint64_t offset = 0;
	segment_table* st = s_malloc(sizeof(segment_table));
	memset(st, 0, sizeof(segment_table));
	// Implementar segment tables acá. Por ahora, manda solo un dato de ejemplo.
	package_decode_buffer(source, &(st->id), &offset);
	return st;
}

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