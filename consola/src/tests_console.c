#include "tests_console.h"

int main() {
	printf("Iniciando Tests...\n\n");
	t_package* package = process_instructions();
	t_queue* instructions = queue_create();
	deserialize_instructions(package, instructions);
	while(!queue_is_empty(instructions)) {
		t_instruction* instruction = queue_pop(instructions);
		printf("Operación: %d\n", instruction->op_code);
		printf("Cantidad de argumentos: %d\n", list_size(instruction->args));
		for(int i = 0; i < list_size(instruction->args); i++)
			printf("Argumento %d: %s\n", i+1, (char*)list_get(instruction->args, i));
		printf("\n");
	}
	printf("\n\nTests Completados\n");
}