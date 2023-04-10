#include "consola.h"


int main(int argc, char ** argv){


    //Inicia el logger
	t_log* logger= iniciar_logger("consola");
    //Carga archivo de configuracion
    t_config* config = iniciar_config("consola");

    //Obtiene la ip y el puerto
    char* ip = config_get_string_value(config, "IP_KERNEL");
	char* puerto = config_get_string_value(config, "PUERTO_KERNEL");
    
    log_info(logger,"El valor de la ip es %s y del puerto es %s \n",ip,puerto);    
    
    int conexion_kernel = socket_initialize_connect(ip, puerto);

	char* mensaje =  "Handsake de consola ready"; 

	socket_send_message( mensaje , conexion_kernel);

    char* leido;
    t_paquete* paquete = create_package(I_O);  //hardcodeado el codigo de operacion con I/O
	// Leemos INSTRUCCIONES
    leido = readline("> ");
	log_info(logger, "Agrego al paquete el primer parametro: %s", leido);
    add_to_package(paquete,leido, strlen(leido + 1));
	free(leido);

	log_info(logger, "Enviando paquete...");

	socket_send_package(paquete, conexion_kernel);
    
    socket_end(conexion_kernel);

    /*
    if(argc > 1 && strcmp(argv[1],"-test")==0)
        return run_tests();
    else{  
        log_info(logger, "Soy la consola! %s", mi_funcion_compartida());
        log_destroy(logger);
    } */


}

void load_basic_config( char* ip,
	                    char* puerto,
                        t_config* config,
                        t_log *logger ) {

    
    
}