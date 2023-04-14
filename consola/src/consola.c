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
    
    t_list *instruccions = list_create(); //Creamos una lista para almacenar las instrucciones

    
    int conexion_kernel = socket_initialize_connect(ip, puerto);

	char* mensaje =  "Handsake de consola ready"; 
	socket_send_message( mensaje , conexion_kernel);

    
    list_instruccions(instruccions); //leemos el archivo y listamos instrucciones
    process_instruccions(instruccions, conexion_kernel); //procesamos la cantidad de parametros y la agregamos al struct correspondiente


    /*
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
    */
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

void list_instruccions(t_list *instruccions)
{
    char *directorio = getcwd(NULL, 0);
    char *file_directory = malloc(strlen(directorio) + strlen("/cfg/instrucciones") + 1);
    sprintf(file_directory, "%s/cfg/instrucciones", directorio);
    
    FILE *file_instruccions;
    file_instruccions = fopen(file_directory, "r");
    if (file_instruccions == NULL) {
        printf("Error al abrir el archivo\n");
        exit(1);
    }
   
    char buffer[256];
    while(fgets(buffer,sizeof(buffer),file_instruccions)){
        char *line = strdup(buffer); // crear una copia de la línea
        //printf("lei: %s",line);
        list_add(instruccions,line); //agrega los elementos leidos a la lista de instrucciones
    }
    

  
    fclose(file_instruccions);
    free(file_directory);
}


void process_instruccions(t_list* instruccions, int socket_cliente){ //con esto procesamos y dependiendo de la cantidad de espacios declaramos la estructura correspondiendte de cada instruccion
     for (int i = 0; i < list_size(instruccions); i++) {
        char* instruccion = list_get(instruccions, i);
        int spaces = count_spaces(instruccion);//cuenta los espacios y dependiendo los espacios sera la cantidad de parametros 
        
        switch (spaces) {
            case 0:
                socket_send_0_params(instruccion,spaces,socket_cliente);
                break;
            case 1:
                break;
            case 2:
                break;
            case 3:
                break;
            default:
                break;
        }
    }

}

int count_spaces(char *instruction_text) { //con esta funcion obtenemos la cantidad de espacios que tiene cada instrucciuon, y asi saber que tipo de estructura tiene
    int count = 0;
    for(int i = 0; i < strlen(instruction_text); i++) {
        if(instruction_text[i] == ' ') {
            count++;
        }
    }
    return count;
}