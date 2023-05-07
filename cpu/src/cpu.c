#include "cpu.h"

configuration_cpu config_cpu;

int main(int argc, char** argv) {
	printf("Iniciando la CPU\n");

	sem_init(&(config_cpu.flag_running), 0, 1);
	sem_init(&(config_cpu.flag_dislodge), 0, 1);

	t_log* logger = start_logger("cpu");
	t_config* config = start_config("cpu");

	config_cpu.logger = logger;

	//int kernel_fd = receive_modules(logger, config);
	// int conn_memoria = connect_module(config,logger,"MEMORIA");

	// TODO: En el primer recv llega basura no se porque
	//socket_receive_message(kernel_fd);
	//socket_receive_message(kernel_fd);

	// To do: CPU debe recibir solo execution context, no todo este PCB

	// Creo el t_pcb para las instrucciones
	t_queue* instructions;
	t_pcb* pcb_test = malloc(sizeof(t_pcb));

	t_list* sublist1 = list_create();
	list_add(sublist1, SET);	
	list_add(sublist1, "AX");
	list_add(sublist1, "HOLA");
	
	t_list* sublist2 = list_create();
	list_add(sublist2, YIELD);
	
	t_list* sublist3 = list_create();
	list_add(sublist3, EXIT);
	
	instructions = list_create();
	list_add(instructions, sublist1);
	list_add(instructions, sublist2);
	list_add(instructions, sublist3);

	pcb_test->state = NEW;
	pcb_test->pid = 6;

	execution_context* context = malloc(sizeof(execution_context));
	context->instructions = instructions;
	context->program_counter = 0;
   	context->updated_state = NEW;
   	context->cpu_register = malloc(sizeof(cpu_register)); // inicializa el puntero
   	context->segment_table = NULL;
	
	pcb_test->execution_context = context;

	// TODO: sigo completando el t_pcb de este proceso
	log_info(logger, "El proceso %d se creó en NEW\n", pcb_test->pid);

	// Recibe los pcbs que aca están harcodeados y los opera
	fetch(pcb_test);

	free(context);
	free(pcb_test);
}