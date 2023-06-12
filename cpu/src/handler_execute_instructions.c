#include "handler_execute_instructions.h"

void execute_set(execution_context* execution_context, t_instruction* instruction) {
	
	// almaceno el nombre del registro y el valor
	char* register_name = list_get(instruction->args, 0);
	// lo paso a un int con atoi el valor proporcionado en la lista
	char* value = list_get(instruction->args, 1);

	printf("Seteando un %s en %s\n", value,register_name);
	// comparo el registro y asigno el valor correspondiente
	cpu_register* registers = execution_context->cpu_register;

    if (strcmp(register_name, "AX") == 0) {
        set_register_value(register_name, value, registers->register_4.AX, sizeof(registers->register_4.AX));
    } else if (strcmp(register_name, "BX") == 0) {
        set_register_value(register_name, value, registers->register_4.BX, sizeof(registers->register_4.BX));
    } else if (strcmp(register_name, "CX") == 0) {
        set_register_value(register_name, value, registers->register_4.CX, sizeof(registers->register_4.CX));
    } else if (strcmp(register_name, "DX") == 0) {
        set_register_value(register_name, value, registers->register_4.DX, sizeof(registers->register_4.DX));
    } else if (strcmp(register_name, "EAX") == 0) {
        set_register_value(register_name, value, registers->register_8.EAX, sizeof(registers->register_8.EAX));
    } else if (strcmp(register_name, "EBX") == 0) {
        set_register_value(register_name, value, registers->register_8.EBX, sizeof(registers->register_8.EBX));
    } else if (strcmp(register_name, "ECX") == 0) {
        set_register_value(register_name, value, registers->register_8.ECX, sizeof(registers->register_8.ECX));
    } else if (strcmp(register_name, "EDX") == 0) {
        set_register_value(register_name, value, registers->register_8.EDX, sizeof(registers->register_8.EDX));
    } else if (strcmp(register_name, "RAX") == 0) {
        set_register_value(register_name, value, registers->register_16.RAX, sizeof(registers->register_16.RAX));
    } else if (strcmp(register_name, "RBX") == 0) {
        set_register_value(register_name, value, registers->register_16.RBX, sizeof(registers->register_16.RBX));
    } else if (strcmp(register_name, "RCX") == 0) {
        set_register_value(register_name, value, registers->register_16.RCX, sizeof(registers->register_16.RCX));
    } else if (strcmp(register_name, "RDX") == 0) {
        set_register_value(register_name, value, registers->register_16.RDX, sizeof(registers->register_16.RDX));
    } else {
        printf("Registro no válido\n");
    }
}

void set_register_value(char* register_name, char* value, void* register_ptr, size_t register_size) {
	if (register_name == NULL || value == NULL || register_ptr == NULL || register_size == 0) {
        printf("Error: Argumentos inválidos\n");
        return;
    }
	size_t value_len = strlen(value);
    if (value_len > register_size) {
		printf("Error: tamaño del registro -> %lu\n",value_len);
		printf("Error: tamaño del value_len -> %lu\n",register_size);
        printf("Error: El valor excede el tamaño del registro\n");
        return;
    }
    strncpy(register_ptr, value, register_size - 1);
    ((char*)register_ptr)[register_size - 1] = '\0';
}

void execute_exit(execution_context* execution_context) {
	execution_context->updated_state = EXIT_PROCESS;
}