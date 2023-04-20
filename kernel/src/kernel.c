#include "kernel.h"


int main(int argc, char ** argv){

    printf("Iniciando el kernel");
    
    t_log* logger = iniciar_logger("kernel"); 

    t_config* config = iniciar_config("kernel");

    //Obtenemos el puerto con el que escucharemos conexiones
	char* puerto = config_get_string_value(config, "PUERTO_ESCUCHA");
    
    log_info(logger,"El valor del puerto es %s \n",puerto);    

    //Inicializo el socket en el puerto cargado por la config
    int server_fd = socket_initialize(puerto);
    log_info(logger,"SOCKET INICIALIZADO");    
    //Pongo el socket en modo de aceptar las escuchas
	int cliente_fd = socket_accept(server_fd);
	int recive_instrucction = instruction_handler_reciver(cliente_fd);
    if( recive_instrucction == (-1) ) {
        printf("Hubo un error !!! "); //esto deberia pasar al logger   --TODO
    }
    //Si el proceso o los procesos terminan de ejecutarse envio un mensaje de ok,
    //if() { 
    t_paquete* paquete = create_package(OK);
    socket_send_package(paquete, cliente_fd);
    /*}else{
        t_paquete* paquete = create_package("ERROR_RI");
    	socket_send_package(paquete, cliente_fd);
    }*/
}