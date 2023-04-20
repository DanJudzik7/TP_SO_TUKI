#include "cpu.h"
int main(int argc, char ** argv){

    printf("Iniciando la CPU");

    t_log* logger = iniciar_logger("cpu");

    t_config* config = iniciar_config("cpu");

    //Obtenemos el puerto con el que escucharemos conexiones
	char* puerto = config_get_string_value(config, "PUERTO_ESCUCHA");

    log_info(logger,"El valor del puerto es %s \n",puerto);    

    //Inicializo el socket en el puerto cargado por la config
    int server_fd = socket_initialize(puerto);
    log_info(logger,"SOCKET INICIALIZADO");    
    //Pongo el socket en modo de aceptar las escuchas
	int cliente_fd = socket_accept(server_fd);
    
}