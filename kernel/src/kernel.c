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
    //Pongo el socket en modo de aceptar las escuchas
	int cliente_fd = socket_accept(server_fd);

    t_list* list_instructions;

	while (1) {
		int cod_op = socket_recv_operation(cliente_fd);
        log_info(logger,"El codigo de operacion es: %i",cod_op);
		switch (cod_op) {
		case MENSAJE:
			socket_recv_message(cliente_fd);
			break;
        case F_READ:
        case F_WRITE:
			list_instructions = socket_recv_package(cliente_fd);
            log_info(logger, "Me llegaron los siguientes valores:\n");
			list_iterate(list_instructions, (void*) iterator);
			break;
        case SET:
        case MOV_IN:
        case MOV_OUT:
        case F_TRUNCATE:
        case F_SEEK:
        case CREATE_SEGMENT:
			list_instructions = socket_recv_package(cliente_fd);
            log_info(logger, "Me llegaron los siguientes valores:\n");
			list_iterate(list_instructions, (void*) iterator);
			break;
        case I_O:
        case WAIT:
        case SIGNAL:
        case F_OPEN:
        case F_CLOSE:
        case DELETE_SEGMENT:
			list_instructions = socket_recv_package(cliente_fd);
            log_info(logger, "Me llegaron los siguientes valores:\n");
			list_iterate(list_instructions, (void*) iterator);
			break;  
        case EXIT:
        case YIELD:
			list_instructions = socket_recv_package(cliente_fd);
            log_info(logger, "Me llegaron los siguientes valores:\n");
			list_iterate(list_instructions, (void*) iterator);
			break;  
        case (-1): 
            log_info(logger,"Error al recibir codigo de operacion \n");
            return (-1);
        }
    }
}

//ESTA DE MAS, DESPUES VOLARLA JUNTO CON SU DECLARACION EN .H
void iterator(char* value) {
	printf("%s\n", value);
}