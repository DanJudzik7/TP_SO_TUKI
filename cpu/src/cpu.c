#include "cpu.h"

int main(int argc, char** argv) {
	printf("Iniciando la CPU\n");

	t_log* logger = start_logger("cpu");
	t_config* config = start_config("cpu");

	int kernel_fd = receive_modules(logger, config);
	// int conn_memoria = connect_module(config,logger,"MEMORIA");

	// TODO: En el primer recv llega basura no se porque
	socket_receive_message(kernel_fd);
	socket_receive_message(kernel_fd);

	// To do: CPU debe recibir solo execution context, no todo este PCB

	// Creo el t_pcb para las instrucciones
	t_list* instructions;
	t_pcb* pcb_test = malloc(sizeof(t_pcb));

	t_list* sublist1 = list_create();
	list_add(sublist1, (void*)SET);	 // Debo hacer el casting a void siempre, dado que son enums
	list_add(sublist1, "AX");
	list_add(sublist1, "HOLA");

	t_list* sublist2 = list_create();
	list_add(sublist2, (void*)WAIT);
	list_add(sublist2, "DISCO");

	instructions = list_create();
	list_add(instructions, sublist1);
	list_add(instructions, sublist2);

	pcb_test->state = NEW;
	pcb_test->pid = 6;

	execution_context* context = malloc(sizeof(execution_context));
	context->instructions = &instructions;
	context->program_counter = 0;

	pcb_test->execution_context = context;

	// TODO: sigo completando el t_pcb de este proceso
	log_info(logger, "El proceso %d se creó en NEW\n", pcb_test->pid);

	// Recibe los pcbs que aca están harcodeados y los opera
	instruction_cycle(pcb_test);
	free(context);
	free(pcb_test);
}