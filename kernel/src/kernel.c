
#include "kernel.h"

int main(int argc, char** argv) {
	t_log* logger = start_logger("kernel");
	t_config* config = start_config("kernel");
	log_warning(logger, "Iniciando el kernel");
	char* port = config_get_string_value(config, "PUERTO_ESCUCHA");// Obtenemos el port con el que escucharemos conexiones
	int server_fd = socket_initialize_server(port);// Inicializo el socket en el port cargado por la config
	log_warning(logger, "Socket de servidor inicializado en puerto %s", port);

	// Connect_module conecta al modulo que le pasa como tercer parámetros
	int conn_cpu =  connect_module(config, logger, "CPU");//ME CONECTO CON CPU Y CREO LA CONEXION
	// int conn_memoria = connect_module(config, logger, "MEMORIA");
	// int conn_filesystem = connect_module(config, logger, "FILESYSTEM");

	// Obtengo del config el algoritmo a usar
	char* algorithm = config_get_string_value(config, "ALGORITMO_PLANIFICACION");
	int burst_time = *config_get_string_value(config, "ESTIMACION_INICIAL") - '0';
	log_warning(logger, "El algoritmo de planificación es: %s", algorithm);

	// cola global de pcb en |READY|
	t_queue* queue_global_pcb = queue_create();

	global_config_kernel* gck = malloc(sizeof(global_config_kernel));
	gck->logger = logger;
	gck->global_pcb = queue_global_pcb;
	gck ->conection_kernel = server_fd;
	gck->max_multiprogramming = 1; //ESTO CAMBIARLO Y CARGARLO POR CONFIG
	gck->default_burst_time = burst_time;
	gck->algorithm = algorithm;
	gck->conection_module_cpu = conn_cpu;
	//gck->conection_module_memory = conn_memoria;
	//gck->conection_module_filesystem = conn_filesystem;

	pthread_t thread_cpu;
	

	pthread_create(&thread_cpu, NULL, (void*) listen_cpu, gck);

	//cada vez que me llega un nuevo proceso deberia abrir una config unica para ese proces
	config_current_process* config_current_procces = malloc(sizeof(config_current_process)); 
	pthread_t thread_consola;
	pthread_create(&thread_consola, NULL, (void*) listen_console, config_current_procces);
		


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