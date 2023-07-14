#include "utils_cpu.h"

char* register_pointer(char* name, cpu_register* registers) {
	if (strcmp(name, "AX") == 0)
		return registers->register_4.AX;
	else if (strcmp(name, "BX") == 0)
		return registers->register_4.BX;
	else if (strcmp(name, "CX") == 0)
		return registers->register_4.CX;
	else if (strcmp(name, "DX") == 0)
		return registers->register_4.DX;
	else if (strcmp(name, "EAX") == 0)
		return registers->register_8.EAX;
	else if (strcmp(name, "EBX") == 0)
		return registers->register_8.EBX;
	else if (strcmp(name, "ECX") == 0)
		return registers->register_8.ECX;
	else if (strcmp(name, "EDX") == 0)
		return registers->register_8.EDX;
	else if (strcmp(name, "RAX") == 0)
		return registers->register_16.RAX;
	else if (strcmp(name, "RBX") == 0)
		return registers->register_16.RBX;
	else if (strcmp(name, "RCX") == 0)
		return registers->register_16.RCX;
	else if (strcmp(name, "RDX") == 0)
		return registers->register_16.RDX;
	else
		return NULL;
}

t_physical_address* mmu(int logical_address, int size, execution_context* ec) {
	t_physical_address* physical_address = malloc(sizeof(t_physical_address));
	physical_address->segment = floor(logical_address / config_cpu.max_segment_size);
	physical_address->offset = logical_address % config_cpu.max_segment_size;
	if (physical_address->offset + size > list_get_by_sid(ec->segment_table, physical_address->segment)->offset) {
		log_error(config_cpu.logger, "La dirección lógica resultó en Segmentation Fault");
		free(physical_address);
		return NULL;
	}
	return physical_address;
}

segment* list_get_by_sid(t_list* list, int id) {
	int i = 0;
	while (i < list_size(list)) {
		segment* segment = list_get(list, i);
		if (segment->s_id == id) return segment;
		i++;
	}
	return NULL;
}