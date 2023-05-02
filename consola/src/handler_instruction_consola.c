#include "handler_instruction_consola.h"

void process_instructions_from_file(int socket_kernel) {
	char* directorio = getcwd(NULL, 0);
	char* file_directory = malloc(strlen(directorio) + strlen("/cfg/instrucciones") + 1);
	sprintf(file_directory, "%s/cfg/instrucciones", directorio);

	FILE* instructions;
	instructions = fopen(file_directory, "r");
	if (instructions == NULL) {
		printf("Error al abrir el archivo\n");
		exit(EXIT_FAILURE);
	}

	char buffer[512];
	while (fgets(buffer, sizeof(buffer), instructions)) {
		char* line = strdup(buffer);  // crear una copia de la línea
		line[strcspn(line, "\n")] = '\0';  // Eliminamos el fin de linea
		socket_send_package(package_instruction_build(line), socket_kernel);
		free(line);
	}

	fclose(instructions);
	free(file_directory);
}

int string_array_length(char** array) {
	int count = 0;
	while (array[count] != NULL) count++;
	return count;
}

// Maneja la instrucciones de la linea, y nos devuelve un package
t_package* package_instruction_build(char* line) {
	// Devuelve un array de string de los argumentos
	char** params = string_split(line, " ");
	op_code cod_op = return_opcode(params[0]);
	// Creamos el package con el operation code correspondiente
	t_package* package = package_create(cod_op);
	for (int i = 1; i < string_array_length(params); i++) {
		package_add(package, params[i], strlen(params[i]) + 1);
	}
	return package;
}
// TODO ESTA OPERACIÓN TENDRÍA QUE IR AL SHARED
op_code_reception opcode_receive(char* code) {
	if (strcmp(code, "RECEIVE_OK") == 0) return OK;
	else if (strcmp(code, "ERROR") == 0) return ERROR;
	else return (-1);  // Error handling
}