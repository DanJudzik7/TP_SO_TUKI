#ifndef SERIALIZE_UTILS_KERNEL_H
#define SERIALIZE_UTILS_KERNEL_H


#include "shared_utils.h"

typedef enum
{
	MENSAJE, 				// 0
	F_READ, 				// 1
	F_WRITE,				// 2
	SET,					// 3
	MOV_IN,					// 4
	MOV_OUT,				// 5
	F_TRUNCATE, 			// 6
	F_SEEK,					// 7
	CREATE_SEGMENT,			// 8
	I_O,					// 9
	WAIT,					// 10
	SIGNAL,					// 11
	F_OPEN,					// 12
	F_CLOSE,				// 13	
	DELETE_SEGMENT,			// 14
	EXIT,					// 15
	YIELD					// 16
}op_code;

typedef struct
{
	op_code codigo_operacion;
	t_buffer* buffer;
} t_paquete;


void* serialize_recv_buffer(int* size, int socket_cliente);

t_list* serialize_recv_package(int socket_cliente);

#endif