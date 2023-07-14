#include "tests_filesystem.h"

int main() {
    setup_config();
    printf("Iniciando Tests...\n\n");

    test_bf_ip();
    //dynamic_op_test();

    printf("\n\nTests Completados\n");
	return 0;
}

void test_bf_ip() {
	t_list* numbers = list_create();
	int* number1 = malloc(sizeof(int));
	*number1 = 2;
	int* number2 = malloc(sizeof(int));
	*number2 = 3;
	int* number3 = malloc(sizeof(int));
	*number3 = 4;
	int* number4 = malloc(sizeof(int));
	*number4 = 32;
	list_add(numbers, number1);
	list_add(numbers, number2);
	list_add(numbers, number3);
	list_add(numbers, number4);
	set_bf_ip(1, numbers);
	get_bf_ip(1);
}

void dynamic_op_test() {
	t_instruction* instruction_new = malloc(sizeof(t_instruction));
	//instruction_new->op_code = F_OPEN;
	//instruction_new->op_code = F_CLOSE;
	//instruction_new->op_code = F_TRUNCATE;
	//instruction_new->op_code = F_READ;
	instruction_new->op_code = F_WRITE;

	instruction_new->args = list_create();
	/*char* argument = strdup("prueba3");
	list_add(instruction_new->args, argument);*/
	char* argument1 = strdup("123");
	list_add(instruction_new->args, argument1);
	char* argument2 = strdup("456");
	list_add(instruction_new->args, argument2);
	/*char* argument2 = strdup("64"); // TAMAÑO TRUNCATE
	list_add(instruction->args, argument2);*/
	char* argument4 = strdup("32"); // TAMAÑO LEER/ESCRIBIR
	list_add(instruction_new->args, argument4);
	/*char* argument5 = strdup("15"); // POSICIÓN
	list_add(instruction->args, argument5); // POSICIÓN */

	process_instruction(instruction_new);

	/* Lo siguiente creo que ya quedó obsoleto:
	t_package* packaged_send = serialize_instruction_test(instruction_new);
	if (!socket_send(config_fs.socket_memoria, packaged_send)) log_error(config_fs.logger, "Error al enviar instrucciones al memoria");
	else log_warning(config_fs.logger, "OK instrucciones al memoria");
	t_package* package_recibe_memory = socket_receive(config_fs.socket_memoria);
	char* str_write = deserialize_message(package_recibe_memory);
	log_info(config_fs.logger, "El valor de la cadena de memoria es: %s", str_write);
	*/
}