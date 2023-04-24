#include "cpu.h"
int main(int argc, char ** argv){

    printf("Iniciando la CPU\n");

    t_log* logger = iniciar_logger("cpu");
    t_config* config = iniciar_config("cpu");

    int kernel_fd = receive_modules(logger,config);
    int conexion_memoria = conect_modules(config,logger,"MEMORIA");

    //TODO: En el primer recv llega basura no se porque
    socket_recv_message(kernel_fd);
    socket_recv_message(kernel_fd);
    
 //Creo el pcb para las instrucciones
    t_list* instruccions;
    pcb *pcb_test;


    pcb_test -> state_pcb = NEW;
    
    pcb_test -> pid = 2001;
    
    pcb_test -> execution_context -> instruccions = [ [SET, "AX", "HOLA"] ];
    pcb_test -> execution_context-> program_counter = 0;  
    // TODO: sigo completando el pcb de este proceso
    log_info(logger,"Se crea el proceso %d en NEW \n",pcb_test -> pid);
}