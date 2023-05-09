#include "cpu.h"

configuration_cpu config_cpu;

int main(int argc, char** argv) {
	printf("Iniciando la CPU\n");

	sem_init(&(config_cpu.flag_running), 0, 1);
	sem_init(&(config_cpu.flag_dislodge), 0, 1);

	t_log* logger = start_logger("cpu");
	t_config* config = start_config("cpu");

	config_cpu.logger = logger;
	
	char* port = config_get_string_value(config, "PUERTO_ESCUCHA");	
	int socket_cpu = socket_initialize_server(port);	// Inicializo el socket en el puerto cargado por la config
	if (socket_cpu == -1) {
		log_error(logger, "No se pudo inicializar el socket de servidor");
		exit(EXIT_FAILURE);
	}
	log_warning(logger, "Socket de servidor inicializado en puerto %s", port);

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

	//pthread_t thread_consola;
	//pthread_create(&thread_consola, NULL, (void*) listen_kernel, socket_cpu);

	// TODO: sigo completando el t_pcb de este proceso
	log_info(logger, "El proceso %d se creó en NEW\n", pcb_test->pid);

	// Recibe los pcbs que aca están harcodeados y los opera
	int kernel_socket = socket_accept(socket_cpu);
	fetch(context,kernel_socket);

	free(context);
	free(pcb_test);
}

void listen_kernel(int socket_cpu) {

	int sem_value;

	while (1) {
		
		int kernel_socket = socket_accept(socket_cpu);

		if(sem_getvalue(&config_cpu.flag_running, &sem_value)){
			
			t_list* paquete = socket_receive_package(kernel_socket);
			execution_context* context = deserialize_context(paquete); // Implementar
			fetch_args* args = malloc(sizeof(fetch_args));
			args->context = context;
			args->kernel_socket = kernel_socket;
			sem_wait(&config_cpu.flag_running);
			pthread_t thread;
			// Se crea un thread para ejecutar el contexto y sus instrucciones
			pthread_create(&thread, NULL, (void*) fetch , args);
			pthread_join(thread, NULL);
			free(args);
		}
		else{
			//En caso contrario envio un mensaje al kernel de que estoy ocupado
			t_package* package = package_create(BUSY); 
        	socket_send_package(package, kernel_socket);
		}
	}
}