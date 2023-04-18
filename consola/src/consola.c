#include "consola.h"


int main(int argc, char ** argv){

    //Inicia el logger
	t_log* logger= iniciar_logger("consola");
    //Carga archivo de configuracion
    t_config* config = iniciar_config("consola");

    char buffer[1024] = {0};
    //Obtiene la ip y el puerto
    char* ip = config_get_string_value(config, "IP_KERNEL");
	char* puerto = config_get_string_value(config, "PUERTO_KERNEL");
    log_info(logger,"El valor de la ip es %s y del puerto es %s \n",ip,puerto);    
    
    //Creamos una lista para almacenar las instrucciones
    t_list *instruccions = list_create(); 
    list_instruccions(instruccions);
    int conexion_kernel = socket_initialize_connect(ip, puerto);
	char* mensaje =  "Handsake de consola ready"; 
    
	socket_send_message( mensaje , conexion_kernel);

    //leemos el archivo y lo mandamos a un handler instruction que maneja que hacer con lo leido
    process_instruccions(instruccions, conexion_kernel); 

    while(1){
        read(conexion_kernel, buffer, 1024);
        op_code_reception codop_console_recived = opcode_recive(buffer);
        if( codop_console_recived == RECIVE_OK)
        { log_info(logger,"Recibimos un OK por parte del kernel \n");  }
        if( codop_console_recived == ERROR)
        { log_info(logger,"Recibimos un ERROR por parte del kernel \n");  }
    }
}

