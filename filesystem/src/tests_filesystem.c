#include "tests_filesystem.h"

int main() {
    //setup_config();
    printf("Iniciando Tests...\n\n");

	test_instruction_serializing();
    //test_bf_ip();
    //test_dynamic_op();

    printf("\n\nTests Completados\n");
	return 0;
}

void test_bf_ip() {
	t_list* numbers = list_create();
	int* number1 = s_malloc(sizeof(int));
	*number1 = 2;
	int* number2 = s_malloc(sizeof(int));
	*number2 = 3;
	int* number3 = s_malloc(sizeof(int));
	*number3 = 4;
	int* number4 = s_malloc(sizeof(int));
	*number4 = 32;
	list_add(numbers, number1);
	list_add(numbers, number2);
	list_add(numbers, number3);
	list_add(numbers, number4);
	set_bf_ip(1, numbers);
	get_bf_ip(1);
}

void test_instruction_serializing() {
	t_instruction* instruction = instruction_new(F_WRITE);
	char* argument0 = strdup("prueba3");
	list_add(instruction->args, argument0);
	char* argument1 = strdup("123");
	list_add(instruction->args, argument1);
	char* argument2 = strdup("456");
	list_add(instruction->args, argument2);
	char* argument4 = strdup("32");
	list_add(instruction->args, argument4);

	t_package* packaged = serialize_instruction(instruction);
	t_instruction* instruction_deserialized = deserialize_instruction(packaged);
	printf("Instrucción op_code: %d", instruction_deserialized->op_code);
}

void test_dynamic_op() {
	// F_OPEN, F_CLOSE, F_TRUNCATE, F_READ, F_WRITE
	t_instruction* instruction = instruction_new(F_WRITE);

	instruction->args = list_create();
	/*char* argument0 = strdup("prueba3");
	list_add(instruction->args, argument0);*/
	char* argument1 = strdup("123");
	list_add(instruction->args, argument1);
	char* argument2 = strdup("456");
	list_add(instruction->args, argument2);
	/*char* argument2 = strdup("64"); // TAMAÑO TRUNCATE
	list_add(instruction->args, argument2);*/
	char* argument4 = strdup("32"); // TAMAÑO LEER/ESCRIBIR
	list_add(instruction->args, argument4);
	/*char* argument5 = strdup("15"); // POSICIÓN
	list_add(instruction->args, argument5); // POSICIÓN */

	process_instruction(instruction);
}