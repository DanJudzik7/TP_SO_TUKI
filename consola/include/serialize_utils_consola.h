#ifndef SERIALIZE_UTILS_CONSOLA_H
#define SERIALIZE_UTILS_CONSOLA_H

#include "shared_utils.h"
#include "socket_utils_consola.h"

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

typedef enum{
	ERROR,
	RECIVE_OK
} op_code_reception;

typedef struct
{
	op_code codigo_operacion;
	t_buffer* buffer;
} t_paquete;


void* serialize_package(t_paquete* paquete, int bytes);

void add_to_package(t_paquete* paquete, void* valor, int tamanio);

t_paquete* create_package(int cod_op);

void delete_package(t_paquete* paquete);

void create_buffer(t_paquete* paquete);


#endif