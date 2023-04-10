#ifndef SERIALIZE_UTILS_KERNEL_H
#define SERIALIZE_UTILS_KERNEL_H


#include "shared_utils.h"

typedef enum
{
	MENSAJE,
	F_READ,
	F_WRITE,
	SET,
	MOV_IN,
	MOV_OUT,
	F_TRUNCATE,
	F_SEEK,
	CREATE_SEGMENT,
	I_O,
	WAIT,
	SIGNAL,
	F_OPEN,
	F_CLOSE,
	DELETE_SEGMENT,
	EXIT,
	YIELD
}op_code;

typedef struct
{
	op_code codigo_operacion;
	t_buffer* buffer;
} t_paquete;


void* serialize_recv_buffer(int* size, int socket_cliente);

t_list* serialize_recv_package(int socket_cliente);

#endif