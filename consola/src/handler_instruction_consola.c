#include "handler_instruction_consola.h"

t_package* process_instructions(int socket_kernel) {
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
		line[strlen(line) - 1] = '\0';
		package_nest(package, parse_instruction(line));
	}
	fclose(instructions);
	return package;
}