#include "tests_filesystem.h"

int main() {
	setup_config(false);
	printf("Iniciando Tests...\n\n");

	test_instruction_serializing();
	// test_bf_ip();
	test_full_file_op();

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
	list_add(instruction->args, strdup("prueba3"));
	list_add(instruction->args, strdup("64"));
	list_add(instruction->args, strdup("456"));
	list_add(instruction->args, strdup("32"));

	t_package* packaged = serialize_instruction(instruction);
	t_instruction* instruction_deserialized = deserialize_instruction(packaged);
	printf("Instrucción op_code: %d", instruction_deserialized->op_code);
}

void test_full_file_op() {
	t_instruction* ins_open = instruction_new(F_OPEN);
	list_add(ins_open->args, strdup("prueba"));
	process_instruction(ins_open);
	instruction_destroy(ins_open);

	t_instruction* ins_trunc = instruction_new(F_TRUNCATE);
	list_add(ins_trunc->args, strdup("prueba"));
	list_add(ins_trunc->args, strdup("64"));
	process_instruction(ins_trunc);
	instruction_destroy(ins_trunc);

	t_instruction* ins_rw = instruction_new(F_WRITE);
	list_add(ins_rw->args, strdup("prueba"));
	list_add(ins_rw->args, strdup("25")); // Posición Read/Write
	list_add(ins_rw->args, strdup("32")); // Tamaño Read/Write
	list_add(ins_rw->args, strdup("64")); // PID
	list_add(ins_rw->args, strdup("32")); // Segment ID
	list_add(ins_rw->args, strdup("32")); // Offset
	list_add(ins_rw->args, strdup("SonyPlaystation5SonyPlaystation4"));
	process_instruction(ins_rw);

	ins_rw->op_code = F_READ;
	process_instruction(ins_rw);
	instruction_destroy(ins_rw);

	t_instruction* ins_close = instruction_new(F_CLOSE);
	list_add(ins_close->args, strdup("prueba"));
	process_instruction(ins_close);
	instruction_destroy(ins_close);
}