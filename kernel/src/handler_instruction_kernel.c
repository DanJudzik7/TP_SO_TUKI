#include "handler_instruction_kernel.h"


int instruction_handler_kernel()
{

    while (1)
    {
        int cod_op = 0; // REVISAR 
        switch (cod_op) {
        case F_READ:
        case F_WRITE:
            
            break;
        case SET:
        case MOV_IN:
        case MOV_OUT:
        case F_TRUNCATE:
        case F_SEEK:
        case CREATE_SEGMENT:
        
            break;
        case I_O:
        case WAIT:
        case SIGNAL:
        case F_OPEN:
        case F_CLOSE:
        case DELETE_SEGMENT:

            break;
        case EXIT:
        case YIELD:
        
            break;
        case (-1):
            printf("\nError al recibir codigo de operacion \n");
            return (-1);
        }
    }
}

int instruction_handler_reciver_not_funcional(int cliente_fd){
 
 t_list* payload;

	while (1) {
		int cod_op = socket_recv_operation(cliente_fd);
        printf("\nEl codigo de operacion es -> %i\n",cod_op);
		if ( cod_op == -1 ) {
            printf("\nError al recibir codigo de operacion \n");
           return (-1);
        }
        else if( cod_op == MENSAJE ) {
             socket_recv_message(cliente_fd);
        }
        else { 
            payload = socket_recv_package(cliente_fd);
            //list_iterate(list_instructions, (void*) iterator);
        }
            
    }
}

t_list* instruction_handler_reciver(int cliente_fd,t_log* logger){ 
/*
    t_list* payload;

	while (1) {
		int cod_op = socket_recv_operation(cliente_fd);
        printf("\nEl codigo de operacion es -> %i\n",cod_op);
        payload = socket_recv_package(cliente_fd);
        list_iterate(payload, (void*) iterator);
        printf("\nEl codigo de operacion es -> %i\n",cod_op);
        for (int i = 0; i < list_size(payload); i++) {
            //Devuelve la linea de instruccion 
            char*  instruccion = list_get(payload, i);
            printf(instruccion);
        }*/
        
    t_list* payload;
    t_list* instruccions = list_create();
    t_queue* queue = queue_create();

	while (1) {
		int cod_op = socket_recv_operation(cliente_fd);
        int flag = 0;
        printf("\nEl codigo de operacion es -> %i\n",cod_op);
		switch (cod_op) {
            case MENSAJE:
                flag = socket_recv_message(cliente_fd);
                break;
            case (-1): 
                log_error(logger,"Hubo un error recibiendo las instrucciones \n"); 
                exit(1);
            default: 
                payload = socket_recv_package(cliente_fd);  
                push(queue, payload);   
                //list_iterate(payload, (void*) iterator);
                //list_add(instruccions,payload);
                break;
           }
        if(flag == 1){
            //Si recibe el mensaje de finalizacion de instrucciones, avisa que las recibio y las devuelve en forma de lista
            char* confirmation = "Instrucciones recibidas por el Kernel ";
            socket_send_message(confirmation, cliente_fd);
            return queue;
        }
     }
 }

 
void iterator(char* value) {
	printf("%s\n", value);
}

//debemos hacer que cada vez que recibimos un payload,
//esto se encarga de crear una cola, y va cargando una lista de instrucciones [ ["AUX", Arg1, Arg2] , [] ]
//Para dps si lo enviamos a la cpu podemos saber el tamaño de cada arg de la lista :D
void list_instructions(char *instructions)
{
    
    
}

