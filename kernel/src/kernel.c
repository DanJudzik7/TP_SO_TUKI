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

    //Obtengo del config el algoritmo a usar
    char* algorithm = config_get_string_value(config,"ALGORITMO_PLANIFICACION");

    //Pongo el socket en modo de aceptar las escuchas
	int consola_fd = socket_accept(server_fd);
	t_queue* queue_instructons = instruction_handler_reciver(consola_fd,logger);
    
    log_info(logger, "El algoritmo de planficiacion es: %s\n",algorithm);

    //Creo el pcb para las instrucciones
    t_list* instructions;
    pcb* pcb_test = malloc(sizeof(pcb));
    

                t_list* sublist1 = list_create();
                list_add(sublist1,(void *) SET);  // Debo hacer el casteo a void siempre, dado que son enums
                list_add(sublist1, "AX");
                list_add(sublist1, "120");

                t_list* sublist2 = list_create();
                list_add(sublist2, (void *) WAIT);
                list_add(sublist2, "DISCO");

                instructions = list_create();
                list_add(instructions, sublist1);
                list_add(instructions, sublist2);


    pcb_test -> state_pcb = NEW;
    pcb_test -> pid = 2001;


    execution_context* context = malloc(sizeof(execution_context));
    context -> instructions = &instructions;
    context -> program_counter = 0;  

    pcb_test->execution_context = context; 

    // TODO: sigo completando el pcb de este proceso
    log_info(logger,"Se crea el proceso %d en NEW \n", pcb_test-> pid);
    
  
    t_queue* queue_global_pcb = queue_create();
    
    pcb* pcb_send =  planficador_kernel(pcb_test,algorithm,queue_global_pcb);
    

    free(context);
    free(pcb_test);


    // Lo comento porque manejo el error y el ok en la funcion
    /*printf(recive_instrucction);
    if( recive_instrucction == (-1) ) {
        log_error(logger,"Hubo un error recibiendo las instrucciones"); 
        exit(1);
    }else if(strcmp(recive_instrucction,"OK_SEND_INSTRUCCTIONS")==0){// EN ESTE IF CREAR Y DEFINIR EL PCB PARA ENVIARLO A CPU, 
        t_paquete* paquete = create_package(OK);
        socket_send_package(paquete, consola_fd);
        
    }*/

    //Si el proceso o los procesos terminan de ejecutarse envio un mensaje de ok,
    //if() { 
    
    /*}else{
        t_paquete* paquete = create_package("ERROR_RI");
    	socket_send_package(paquete, consola_fd);
    }*/
}