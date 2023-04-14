#include "instruction_handler.h"

int instruction_handler_console(int cliente_fd){ 

    t_list* list_instructions;

	while (1) {
		int cod_op = socket_recv_operation(cliente_fd);
        printf("\nEl codigo de operacion es -> %i\n",cod_op);
		switch (cod_op) {
            case MENSAJE:
                socket_recv_message(cliente_fd);
                break;
            case F_READ: 
            case F_WRITE:
                list_instructions = socket_recv_package(cliente_fd);
                //list_iterate(list_instructions, (void*) iterator);
                break;
            case SET:
            case MOV_IN:
            case MOV_OUT:
            case F_TRUNCATE:
            case F_SEEK:
            case CREATE_SEGMENT:
                list_instructions = socket_recv_package(cliente_fd);
                //list_iterate(list_instructions, (void*) iterator);
                break;
            case I_O:
            case WAIT:
            case SIGNAL:
            case F_OPEN:
            case F_CLOSE:
            case DELETE_SEGMENT:
                list_instructions = socket_recv_package(cliente_fd);
                //list_iterate(list_instructions, (void*) iterator);
                break;  
            case EXIT:
            case YIELD:
                list_instructions = socket_recv_package(cliente_fd);
                printf("\nME LLEGO UN EXIT O YIELD:\n");
                //list_iterate(list_instructions, (void*) iterator);
                break;  
            case (-1): 
                printf("\nError al recibir codigo de operacion \n");
                return (-1);
        }
    }
}
