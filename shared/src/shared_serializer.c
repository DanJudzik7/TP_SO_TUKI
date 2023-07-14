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
				//ec->segment_table = deserialize_segment_table(nested_package);
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

void deserialize_single_instruction(t_package* package, t_instruction* instruction) {
	uint64_t offset = 0;
	// Va buscando todas las instrucciones
	while (package_decode_isset(package, offset)) {
		t_package* instruction_package = package_decode(package->buffer, &offset);
		instruction->op_code = instruction_package->type;
		instruction->args = list_create();
		uint64_t offset_list = 0;
		// Carga los args de package_add
		while (package_decode_isset(instruction_package, offset_list)){
			list_add(instruction->args, package_decode_string(instruction_package->buffer, &offset_list));
		}
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


t_package* serialize_segment_table(segment_table* sg) {
	// Crear un nuevo paquete con suficiente tamaño para todos los segmentos y el pid
	t_package* package = package_new(SEGMENT_TABLE);

	uint64_t size = sizeof(uint32_t);
	// Serializar el pid y añadirlo al buffer del paquete
	package_add(package, &(sg->pid),  &size);
	package_add(package, &(sg->s_id), &size);
	package_add(package, &(sg->size_segment), &size);
/*
    for (int i = 0; i < sg->segment_table_pcb->elements_count; i++) {
        segment* seg = list_get(sg->segment_table_pcb, i);
        char* serialized_segment = serialize_segment(seg);
        package_add(package, serialized_segment, sizeof(segment));
        free(serialized_segment);
    }
*/
	return package;
}

char* serialize_segment(segment* seg) {
	char* buffer = malloc(sizeof(segment));
	// Reemplazar estas líneas con la lógica de serialización real para cada campo del segmento
	memcpy(buffer, &(seg->base), sizeof(void*));
	buffer += sizeof(void*);
	memcpy(buffer, &(seg->offset), sizeof(uint32_t));
	buffer += sizeof(uint32_t);
	memcpy(buffer, &(seg->s_id), sizeof(uint32_t));
	return buffer - sizeof(void*) - 2 * sizeof(uint32_t);  // Rebobinar el buffer antes de devolverlo
}

segment_table* deserialize_segment_table(t_package* package) {
	// Creamos la estructura resultante
	segment_table* sg_list_help = malloc(sizeof(segment_table));
	// Creamos la lista de segmentos
	sg_list_help->segment_table_pcb = list_create();

	// Nos movemos a través del paquete buffer y creamos segmentos
	char* temp_buffer = package->buffer;
	
	memcpy(&(sg_list_help->pid), temp_buffer, sizeof(uint32_t));
	temp_buffer += sizeof(uint32_t);
	memcpy(&(sg_list_help->s_id), temp_buffer, sizeof(uint32_t));
	temp_buffer += sizeof(uint32_t);
	memcpy(&(sg_list_help->size_segment), temp_buffer, sizeof(uint32_t));
	temp_buffer += sizeof(uint32_t);

/*
	for (int i = 0; i < (package->size - sizeof(int)) / sizeof(segment); i++) {
       	// Deserializamos el segmento
		segment* new_segment = deserialize_segment(temp_buffer);
       	// Añadimos el segmento a la lista
		list_add(sg_list_help->segment_table_pcb, new_segment);
       	// Avanzamos el buffer temporal
		temp_buffer += sizeof(segment);
    }
*/
	// Devolvemos el resultado
	return sg_list_help;
}

segment* deserialize_segment(char* buffer) {

	segment* new_segment = malloc(sizeof(segment));
	// Reemplazar estas líneas con la lógica de deserialización real para cada campo del segmento
	memcpy(&(new_segment->base), buffer, sizeof(void*));
	buffer += sizeof(void*);
	memcpy(&(new_segment->offset), buffer, sizeof(int));
	buffer += sizeof(int);
	memcpy(&(new_segment->s_id), buffer, sizeof(int));
	return new_segment;
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




						//////////////////////////// 	ZONA TEST 	///////////////////////////
t_package* serialize_instruction_test(t_instruction* instruction) {
    t_package* package = malloc(sizeof(t_package));
    int args_count = list_size(instruction->args);
    int total_size = sizeof(int) + sizeof(op_code) + args_count * sizeof(int); 

    for (int i = 0; i < args_count; i++) {
        char* arg = list_get(instruction->args, i);
        total_size += strlen(arg) + 1; 
    }
    void* buffer = malloc(total_size);
    int offset = 0;

    memcpy(buffer + offset, &(instruction->op_code), sizeof(op_code));
    offset += sizeof(op_code);
    memcpy(buffer + offset, &args_count, sizeof(int));
    offset += sizeof(int);
    for (int i = 0; i < args_count; i++) {
        char* arg = list_get(instruction->args, i);
        int arg_size = strlen(arg) + 1;
        memcpy(buffer + offset, &arg_size, sizeof(int));
        offset += sizeof(int);
        memcpy(buffer + offset, arg, arg_size);
        offset += arg_size;
    }
    package->size = total_size;
    package->type = instruction->op_code;
    package->buffer = buffer;

    return package;
}

t_instruction* deserialize_instruction_test(t_package* package) {
    t_instruction* instruction = malloc(sizeof(t_instruction));
    int offset = 0;
    memcpy(&(instruction->op_code), package->buffer + offset, sizeof(op_code));
    offset += sizeof(op_code);
    int args_count;
    memcpy(&args_count, package->buffer + offset, sizeof(int));
    offset += sizeof(int);
    instruction->args = list_create();
    for (int i = 0; i < args_count; i++) {
        int arg_size;
        memcpy(&arg_size, package->buffer + offset, sizeof(int));
        offset += sizeof(int);
        char* arg = malloc(arg_size);
        memcpy(arg, package->buffer + offset, arg_size);
        offset += arg_size;
        list_add(instruction->args, arg);
    }

    return instruction;
}