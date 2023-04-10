#ifndef SOCKET_UTILS_KERNEL_H
#define SOCKET_UTILS_KERNEL_H


#include "shared_utils.h"

typedef enum
{
	MENSAJE,
	PAQUETE
}op_code;


int socket_initialize(char* puerto);

int socket_accept(int socket_servidor);

int socket_recv_operation(int socket_cliente);

void* socket_recv_buffer(int* size, int socket_cliente);

void socket_recv_message(int socket_cliente);

#endif