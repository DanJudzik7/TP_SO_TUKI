#include "consola.h"


int main(int argc, char ** argv){

    char* ip;
	char* puerto;
    t_config* config;
    t_log *logger ;


    printf("Iniciando la consola");
    //Inicia el logger
	logger= iniciar_logger("consola");
    //Carga archivo de configuracion
    config = iniciar_config("consola");

    //Obtiene la ip y el puerto
    ip = config_get_string_value(config, "IP_KERNEL");
	puerto = config_get_string_value(config, "PUERTO_KERNEL");
    
    log_info(logger,"El valor de la ip es %s y del puerto es %s \n",ip,puerto);    
    
    int conexion_kernel = socket_initialize_connect(ip, puerto);

	log_info(logger, "Conexion establecida con kernel");

	char* mensaje =  "Handsake ready"; 

	socket_send_message( mensaje , conexion_kernel);
    
    socket_end(conexion_kernel);

    /*
    if(argc > 1 && strcmp(argv[1],"-test")==0)
        return run_tests();
    else{  
        log_info(logger, "Soy la consola! %s", mi_funcion_compartida());
        log_destroy(logger);
    } */


}

