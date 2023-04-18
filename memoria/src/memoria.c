#include "memoria.h"
int main(int argc, char ** argv){

    printf("Iniciando la memoria");
    
    t_log* logger = iniciar_logger("memoria"); 

    t_config* config = iniciar_config("memoria");

    //Obtenemos el puerto con el que escucharemos conexiones
	char* puerto = config_get_string_value(config, "PUERTO_ESCUCHA");

    log_info(logger,"El valor del puerto asignado es: %s \n",puerto);    
    
    //Inicializo el socket en el puerto cargado por la config
    int server_fd = socket_initialize(puerto);
    log_info(logger,"\n SOCKET INICIALIZADO");    
    //Pongo el socket en modo de aceptar las escuchas
	int cliente_fd = socket_accept(server_fd);
	int recive_instrucction = instruction_handler_memoria(cliente_fd);

    if( recive_instrucction == (-1) ) {
       log_info(logger,"ERROR AL RECIBIR UNA INSTRUCCION DEL KERNEL");
    }
}