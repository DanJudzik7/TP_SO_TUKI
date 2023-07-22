#include "utils_cpu.h"

char* register_pointer(char* name, t_registers* registers) {
	if (strcmp(name, "AX") == 0)
		return registers->AX;
	else if (strcmp(name, "BX") == 0)
		return registers->BX;
	else if (strcmp(name, "CX") == 0)
		return registers->CX;
	else if (strcmp(name, "DX") == 0)
		return registers->DX;
	else if (strcmp(name, "EAX") == 0)
		return registers->EAX;
	else if (strcmp(name, "EBX") == 0)
		return registers->EBX;
	else if (strcmp(name, "ECX") == 0)
		return registers->ECX;
	else if (strcmp(name, "EDX") == 0)
		return registers->EDX;
	else if (strcmp(name, "RAX") == 0)
		return registers->RAX;
	else if (strcmp(name, "RBX") == 0)
		return registers->RBX;
	else if (strcmp(name, "RCX") == 0)
		return registers->RCX;
	else if (strcmp(name, "RDX") == 0)
		return registers->RDX;
	else
		return NULL;
}
int size_of_register_pointer(char* name, t_registers* registers) {
	if (strcmp(name, "AX") == 0)
		return sizeof(registers->AX);
	else if (strcmp(name, "BX") == 0)
		return sizeof(registers->BX);
	else if (strcmp(name, "CX") == 0)
		return sizeof(registers->CX);
	else if (strcmp(name, "DX") == 0)
		return sizeof(registers->DX);
	else if (strcmp(name, "EAX") == 0)
		return sizeof(registers->EAX);
	else if (strcmp(name, "EBX") == 0)
		return sizeof(registers->EBX);
	else if (strcmp(name, "ECX") == 0)
		return sizeof(registers->ECX);
	else if (strcmp(name, "EDX") == 0)
		return sizeof(registers->EDX);
	else if (strcmp(name, "RAX") == 0)
		return sizeof(registers->RAX);
	else if (strcmp(name, "RBX") == 0)
		return sizeof(registers->RBX);
	else if (strcmp(name, "RCX") == 0)
		return sizeof(registers->RCX);
	else if (strcmp(name, "RDX") == 0)
		return sizeof(registers->RDX);
	else
		return -1;
}

t_physical_address* mmu(int logical_address, int size, t_execution_context* ec) {
	if (size <= 0) {
		log_error(config_cpu.logger, "El tamaño de la dirección lógica es menor o igual a 0");
		return NULL;
	}
	if (size > config_cpu.max_segment_size) {
		log_error(config_cpu.logger, "El tamaño de la dirección lógica es mayor al tamaño máximo de segmento");
		return NULL;
	}
	t_physical_address* physical_address = s_malloc(sizeof(t_physical_address));
	physical_address->segment = floor(logical_address / config_cpu.max_segment_size);
	physical_address->offset = logical_address % config_cpu.max_segment_size;
	if (physical_address->offset + size > list_get_by_sid(ec->segments_table, physical_address->segment)->offset) {
		log_error(config_cpu.logger, "La dirección lógica resultó en Segmentation Fault");
		free(physical_address);
		return NULL;
	}
	return physical_address;
}

t_segment* list_get_by_sid(t_list* list, int id) {
	int i = 0;
	while (i < list_size(list)) {
		t_segment* segment = list_get(list, i);
		if (segment->s_id == id) return segment;
		i++;
	}
	return NULL;
}