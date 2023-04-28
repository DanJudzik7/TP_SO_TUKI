#include "cpu.h"



int main(int argc, char ** argv){

    printf("Iniciando la CPU\n");

    t_log* logger = iniciar_logger("cpu");
    t_config* config = iniciar_config("cpu");

    //int kernel_fd = receive_modules(logger,config);
    //int conexion_memoria = conect_modules(config,logger,"MEMORIA");

    //TODO: En el primer recv llega basura no se porque
    //socket_recv_message(kernel_fd);
    //socket_recv_message(kernel_fd);
    
 //Creo el pcb para las instrucciones
    t_list* instructions;
    pcb* pcb_test = malloc(sizeof(pcb));
    

                t_list* sublist1 = list_create();
                list_add(sublist1,(void *) SET);  // Debo hacer el casteo a void siempre, dado que son enums
                list_add(sublist1, "AX");
                list_add(sublist1, "120");

                t_list* sublist2 = list_create();
                list_add(sublist2, "WAIT");
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
    log_info(logger,"Se crea el proceso %d en NEW \n",pcb_test -> pid);

    //Recibe los pcbs que aca estan harcodeados y los opera
    instruction_cycle(pcb_test);
    free(pcb_test->execution_context);
    free(pcb_test);

}

