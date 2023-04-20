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

int instruction_handler_reciver(int cliente_fd){ 

    t_list* payload;

	while (1) {
		int cod_op = socket_recv_operation(cliente_fd);
        printf("\nEl codigo de operacion es -> %i\n",cod_op);
		switch (cod_op) {
            case MENSAJE:
                socket_recv_message(cliente_fd);
                break;
            case F_READ: 
            case F_WRITE:
                payload = socket_recv_package(cliente_fd);
                //list_iterate(list_instructions, (void*) iterator);
                break;
            case SET:
            case MOV_IN:
            case MOV_OUT:
            case F_TRUNCATE:
            case F_SEEK:
            case CREATE_SEGMENT:
                payload = socket_recv_package(cliente_fd);
                //list_iterate(list_instructions, (void*) iterator);
                break;
            case I_O:
            case WAIT:
            case SIGNAL:
            case F_OPEN:
            case F_CLOSE:
            case DELETE_SEGMENT:
                payload = socket_recv_package(cliente_fd);
                //list_iterate(list_instructions, (void*) iterator);
                break;  
            case EXIT:
            case YIELD:
                payload = socket_recv_package(cliente_fd);
                printf("\nME LLEGO UN EXIT O YIELD:\n");
                //list_iterate(list_instructions, (void*) iterator);
                break;  
            case (-1): 
                printf("\nError al recibir codigo de operacion \n");
                return (-1);
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
    t_queue* queue_create();
    
}

