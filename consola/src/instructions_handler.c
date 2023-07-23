#include "instructions_handler.h"

t_package* process_instructions() {
	char* current_path = getcwd(NULL, 0);
	char* file_path = string_from_format("%s/cfg/instrucciones", current_path);
	FILE* instructions = fopen(file_path, "r");
	free(file_path);
	free(current_path);
	if (instructions == NULL) {
		printf("Error al abrir el archivo\n");
		exit(EXIT_FAILURE);
	}
	t_package* package = package_new(INSTRUCTIONS);
	char* line = s_malloc(512 * sizeof(char));
	while (fgets(line, 512, instructions)) {
		line[strcspn(line, "\r\n")] = '\0';
		if (strlen(line) > 0) package_nest(package, parse_instruction(line));
	}
	free(line);
	fclose(instructions);
	return package;
}

t_package* parse_instruction(char* instruction) {
	char** parts = string_split(instruction, " ");
	// Creamos el package con el operation code correspondiente
	t_package* package = package_new(parse_op_code(parts[0]));
	for (int i = 1; parts[i] != NULL; i++) package_write(package, parts[i]);
	string_iterate_lines(parts, (void*) free);
	free(parts);
	return package;
}

op_code parse_op_code(char* code) {
	if (strcmp(code, "F_READ") == 0)
		return F_READ;
	else if (strcmp(code, "F_WRITE") == 0)
		return F_WRITE;
	else if (strcmp(code, "SET") == 0)
		return SET;
	else if (strcmp(code, "MOV_IN") == 0)
		return MOV_IN;
	else if (strcmp(code, "MOV_OUT") == 0)
		return MOV_OUT;
	else if (strcmp(code, "F_TRUNCATE") == 0)
		return F_TRUNCATE;
	else if (strcmp(code, "F_SEEK") == 0)
		return F_SEEK;
	else if (strcmp(code, "CREATE_SEGMENT") == 0)
		return CREATE_SEGMENT;
	else if (strcmp(code, "I/O") == 0)
		return I_O;
	else if (strcmp(code, "WAIT") == 0)
		return WAIT;
	else if (strcmp(code, "SIGNAL") == 0)
		return SIGNAL;
	else if (strcmp(code, "F_OPEN") == 0)
		return F_OPEN;
	else if (strcmp(code, "F_CLOSE") == 0)
		return F_CLOSE;
	else if (strcmp(code, "DELETE_SEGMENT") == 0)
		return DELETE_SEGMENT;
	else if (strcmp(code, "EXIT") == 0)
		return EXIT;
	else if (strcmp(code, "YIELD") == 0)
		return YIELD;
	else
		return -1;
}