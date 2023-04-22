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

    //Conect_modules conecta al modulo que le envias como tercer parametro
    //int conexion_cpu =  conect_modules(config,logger,"CPU");
    //int conexion_memoria = conect_modules(config,logger,"MEMORIA");
    //int conexion_filesystem = conect_modules(config,logger,"FILESYSTEM");

    
    //Pongo el socket en modo de aceptar las escuchas
	int consola_fd = socket_accept(server_fd);
	int recive_instrucction = instruction_handler_reciver(consola_fd);
    printf(recive_instrucction);
    if( recive_instrucction == (-1) ) {
        log_error(logger,"Hubo un error recibiendo las instrucciones"); 
        exit(1);
    }else if(strcmp(recive_instrucction,"OK_SEND_INSTRUCCTIONS")==0){// EN ESTE IF CREAR Y DEFINIR EL PCB PARA ENVIARLO A CPU, 
        t_paquete* paquete = create_package(OK);
        socket_send_package(paquete, consola_fd);
        
    }
    //Si el proceso o los procesos terminan de ejecutarse envio un mensaje de ok,
    //if() { 
    
    /*}else{
        t_paquete* paquete = create_package("ERROR_RI");
    	socket_send_package(paquete, consola_fd);
    }*/
}