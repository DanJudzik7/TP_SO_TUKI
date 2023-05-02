
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


	t_queue* queue_global_pcb = queue_create();


	t_console_init* ci = malloc(sizeof(t_console_init));

	pthread_t thread_consola;
	ci->logger = logger;
	ci->server_fd = server_fd;
	ci->global_pcb = queue_global_pcb;
	ci->default_burst_time = burst_time;
	ci->algorithm = algorithm;
	pthread_create(&thread_consola, NULL, (void*) listen_console, ci);


	

	pthread_t thread_cpu;
	ci->conection_module = conn_cpu;
	pthread_create(&thread_cpu, NULL, (void*) listen_cpu, ci);
		
	



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