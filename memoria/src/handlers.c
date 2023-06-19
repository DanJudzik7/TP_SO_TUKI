#include "handlers.h"

// No se si conviene hacerlo asi o poner todo en la misma funcion y que todos los hilos la usen
void handle_fs(int socket_fs){
    t_package* fs = socket_receive(socket_fs);
    if (fs == NULL) {
			printf("El cliente se desconectó\n");
			exit(1);
	}

    switch (fs->type)
    {
    //case READ:
        /* code */
        break;
    //case WRITE:       IMPLEMENTAR
        /* code */
        break;   

    default:
        log_error(memory_config.logger,"El proceso recibió algo indebido, finalizando modulo");
        exit(1);
        break;
    }

}

void handle_cpu(int socket_cpu){
    t_package* cpu = socket_receive(socket_cpu);
    if (cpu == NULL) {
			printf("El cliente se desconectó\n");
			exit(1);
	}

    switch (cpu->type)
    {
    //case READ:
        /* code */
        break;
    //case WRITE:       IMPLEMENTAR
        /* code */
        break;   
     
    default:
        log_error(memory_config.logger,"El proceso recibió algo indebido, finalizando modulo");
        exit(1);
        break;
    }
}

void handle_kernel(int socket_kernel){
    t_package* kernel = socket_receive(socket_kernel);
    if (kernel == NULL) {
			printf("El cliente se desconectó\n");
			exit(1);
	}

    switch (kernel->type)
    {
    //case NEW_PROCCESS:
        /* code */
        break;
    //case END_PROCCESS:
        /* code */
        break;
    case CREATE_SEGMENT:
        /* code */
        break;
    case DELETE_SEGMENT:
        /* code */
        break;
    //case COMPACT:
        /* code */
        break;

    default:
        log_error(memory_config.logger,"El proceso recibió algo indebido, finalizando modulo");
        exit(1);
        break;
    }
}