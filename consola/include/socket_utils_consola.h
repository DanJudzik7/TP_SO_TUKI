#ifndef SOCKET_UTILS_CONSOLA_H
#define SOCKET_UTILS_CONSOLA_H


#include "shared_utils.h"

typedef enum
{
	MENSAJE,
}op_code;

typedef struct
{
	int size;
	void* stream;
} t_buffer;

typedef struct
{
	op_code codigo_operacion;
	t_buffer* buffer;
} t_paquete;


int socket_initialize_connect(char *ip,char *puerto);

void* serialize_package(t_paquete* paquete, int bytes);

void socket_send_message(char* mensaje, int socket_cliente);

void delete_package(t_paquete* paquete);

void socket_end(int socket_cliente);

#endif