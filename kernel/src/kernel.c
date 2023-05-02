#include "kernel.h"

int main(int argc, char** argv) {
	t_log* logger = start_logger("kernel");
	t_config* config = start_config("kernel");
	log_warning(logger, "Iniciando el kernel");
	// Obtenemos el port con el que escucharemos conexiones
	char* port = config_get_string_value(config, "PUERTO_ESCUCHA");
	// Inicializo el socket en el port cargado por la config
	int server_fd = socket_initialize_server(port);
	log_warning(logger, "Socket de servidor inicializado en puerto %s", port);

	// Connect_module conecta al modulo que le pasa como tercer parámetros
	// int conn_cpu =  connect_module(config, logger, "CPU");
	// int conn_memoria = connect_module(config, logger, "MEMORIA");
	// int conn_filesystem = connect_module(config, logger, "FILESYSTEM");

	// Obtengo del config el algoritmo a usar
	char* algorithm = config_get_string_value(config, "ALGORITMO_PLANIFICACION");
	int burst_time = *config_get_string_value(config, "ESTIMACION_INICIAL") - '0';
	t_queue* queue_global_pcb = queue_create();

	log_warning(logger, "El algoritmo de planificación es: %s", algorithm);

	// PCB de Instrucciones hardcodeadas
	// Por qué esto sigue acá? Si es de testing de CPU, no es mejor que quede solo ahí?
	t_queue* instructions;
	t_pcb* pcb_test = malloc(sizeof(t_pcb));

	t_list* sublist1 = list_create();
	list_add(sublist1, (void*)SET);	 // Debo hacer el casting a void siempre, dado que son enums
	list_add(sublist1, "AX");
	list_add(sublist1, "120");

	t_list* sublist2 = list_create();
	list_add(sublist2, (void*)WAIT);
	list_add(sublist2, "DISCO");

	instructions = queue_create();
	queue_push(instructions, sublist1);
	queue_push(instructions, sublist2);

	pcb_test->state_pcb = NEW;
	pcb_test->pid = 2001;
	pcb_test->aprox_burst_time = 20;

	pcb_test->execution_context = malloc(sizeof(execution_context));
	pcb_test->execution_context->instructions = instructions;
	pcb_test->execution_context->program_counter = 0;

	// TODO: sigo completando el t_pcb de este proceso
	log_debug(logger, "Se crea el proceso %d en NEW", pcb_test->pid);

	t_pcb* pcb_send = malloc(sizeof(t_pcb));
	pcb_send = planificador_kernel(logger, pcb_test, algorithm, queue_global_pcb);
	// Lo anterior va a ser migrado a un thread que se va a encargar de planificar y de conectar a cpu
	log_debug(logger, "El ID del PCB devuelto es %d", pcb_send->pid);
	free(pcb_test->execution_context);
	// free(pcb_test);

	// Una vez que el planificador está listo, se abre el socket.
	while (1) {
		pthread_t thread;
		int *console_socket = malloc(sizeof(int));
		// Pongo el socket en modo de aceptar las escuchas
		*console_socket = socket_accept(server_fd);
		if (*console_socket == (-1)) {
			log_warning(logger, "Hubo un error aceptando la conexión");
			continue;
		}
		log_debug(logger, "Se aceptó una conexión en el socket %d", *console_socket);
		t_console_init* ci = malloc(sizeof(t_console_init));
		ci->logger = logger;
		ci->console = *console_socket;
		ci->global_pcb = queue_global_pcb;
		ci->default_burst_time = burst_time;
		// Al llegar, lo mando a un thread nuevo
		pthread_create(&thread, NULL, (void*) create_pcb, ci);
		pthread_detach(thread);
	}

	// Lo comento porque manejo el error y el ok en la función
	/*printf(receive_instruction);
	if( receive_instruction == (-1) ) {
		log_error(logger,"Hubo un error recibiendo las instrucciones");
		exit(EXIT_FAILURE);
	}else if(strcmp(receive_instruction,"OK_SEND_INSTRUCTIONS")==0){// EN ESTE IF CREAR Y DEFINIR EL PCB PARA ENVIARLO A CPU,
		t_package* paquete = package_create(OK);
		socket_send_package(paquete, consola_fd);

	}*/

	// Si el proceso o los procesos terminan de ejecutarse envió un mensaje de ok,
	// if() {

	/*}else{
		t_package* paquete = package_create("ERROR_RI");
		socket_send_package(paquete, consola_fd);
	}*/
}