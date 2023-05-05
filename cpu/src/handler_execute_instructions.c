#include "handler_execute_instructions.h"

void set(execution_context* execution_context, t_list* instruction) {
	// almaceno el nombre del registro y el valor
	char* register_name = list_get(instruction, 1);
	// lo paso a un int con atoi el valor proporcionado en la lista
	char* value = list_get(instruction, 2);

	printf("El valor es %s\n", value);
	// comparo el registro y asigno el valor correspondiente
	cpu_register* registers = &execution_context->cpu_register;

	if (strcmp(register_name, "AX") == 0) {
		strcpy(registers->register_4.AX, value);
	} else if (strcmp(register_name, "BX") == 0) {
		strcpy(registers->register_4.BX, value);
	} else if (strcmp(register_name, "CX") == 0) {
		strcpy(registers->register_4.CX, value);
	} else if (strcmp(register_name, "DX") == 0) {
		strcpy(registers->register_4.DX, value);
	} else if (strcmp(register_name, "EAX") == 0) {
		strcpy(registers->register_8.EAX, value);
	} else if (strcmp(register_name, "EBX") == 0) {
		strcpy(registers->register_8.EBX, value);
	} else if (strcmp(register_name, "ECX") == 0) {
		strcpy(registers->register_8.ECX, value);
	} else if (strcmp(register_name, "EDX") == 0) {
		strcpy(registers->register_8.EDX, value);
	} else if (strcmp(register_name, "RAX") == 0) {
		strcpy(registers->register_16.RAX, value);
	} else if (strcmp(register_name, "RBX") == 0) {
		strcpy(registers->register_16.RBX, value);
	} else if (strcmp(register_name, "RCX") == 0) {
		strcpy(registers->register_16.RCX, value);
	} else if (strcmp(register_name, "RDX") == 0) {
		strcpy(registers->register_16.RDX, value);
	} else {
		printf("Registro no válido\n");
	}
	printf("El valor del registro %s es %s\n", register_name, registers->register_4.AX);
}

void yield(t_pcb* t_pcb) {
}

void exitIns(t_pcb* t_pcb) {
}

void dislodge(t_pcb* t_pcb) {
}