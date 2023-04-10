#include "kernel.h"


int main(int argc, char ** argv){

    char* puerto;
    t_config* config;


    printf("Iniciando el kernel");

    t_log* logger = iniciar_logger("kernel"); 

    config = iniciar_config("kernel");

    //Obtenemos el puerto con el que escucharemos conexiones
	puerto = config_get_string_value(config, "PUERTO_ESCUCHA");
    
    log_info(logger,"El valor del puerto es %s \n",puerto);    


    //Inicializo el socket en el puerto cargado por la config
    int server_fd = socket_initialize(puerto);
    log_info(logger,"SOCKET INICIALIZADO");    
    //Pongo el socket en modo de aceptar las escuchas
	int cliente_fd = socket_accept(server_fd);

	while (1) {
		int cod_op = socket_recv_operation(cliente_fd);
		switch (cod_op) {
		case MENSAJE:
			socket_recv_message(cliente_fd);
			break;
        }       
    }
}