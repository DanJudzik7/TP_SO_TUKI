#ifndef SHARED_UTILS_H
#define SHARED_UTILS_H

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <commons/config.h>
#include <commons/collections/list.h>
#include <commons/string.h>
#include <commons/log.h>
#include <stdbool.h>
#include <netdb.h>
#include <sys/socket.h>
#include <readline/readline.h>

typedef enum
{
	MENSAJE, 				// 0     //Este existe de mas
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

typedef enum{
	ERROR,
	RECIVE_OK
} op_code_reception;

typedef struct
{
	uint32_t size;
	void* stream;
} t_buffer;

typedef struct
{
	op_code codigo_operacion;
	t_buffer* buffer;
} t_paquete;

char* obtener_cfg_type(char* process_name,char* file_type);
t_config* iniciar_config(char* process_name);
t_log* iniciar_logger(char* process_name);

/* Inicializa un socket servidor y lo vincula a un puerto específico. Retorna el descriptor del socket creado. */
int socket_initialize(char* puerto);

/* Espera y acepta la conexión entrante de un cliente. Retorna el descriptor del socket del cliente conectado. */
int socket_accept(int socket_servidor); 

/* Inicializa y establece la conexión con un servidor en una dirección IP y puerto específicos. Retorna el descriptor del socket creado. */
int socket_initialize_connect(char *ip, char *puerto); 

/* Recibe y retorna el código de operación enviado por un cliente a través del socket especificado. */
int socket_recv_operation(int socket_cliente); 

/* Recibe un buffer de datos enviado por un cliente a través del socket especificado y retorna un puntero al buffer recibido. También actualiza la variable size con el tamaño del buffer recibido. */
void* socket_recv_buffer(int* size, int socket_cliente); 

/* Envia un mensaje a un cliente a través del socket especificado. */
void socket_send_message(char* mensaje, int socket_cliente); 

/* Recibe y muestra un mensaje enviado por un cliente a través del socket especificado. */
void socket_recv_message(int socket_cliente); 

/* Envía un paquete a un cliente a través del socket especificado. */
void socket_send_package(t_paquete* paquete, int socket_cliente); 

/* Cierra la conexión del socket especificado. */
void socket_end(int socket_cliente); 

/* Crea y retorna un paquete con el código de operación especificado. */
t_paquete* create_package(int cod_op); 

/* Agrega un valor con un tamaño específico al paquete especificado. */
void add_to_package(t_paquete* paquete, void* valor, int tamanio); 

/* Serializa el paquete especificado en un buffer de bytes de tamaño específico y retorna un puntero al buffer. */
void* serialize_package(t_paquete* paquete, int bytes); 

/* Deserializa un paquete a partir de un buffer de bytes recibido. */
void* deserialize_package(); 

/* Libera la memoria asignada a un paquete especificado. */
void delete_package(t_paquete* paquete); 

/* Crea un buffer para el paquete especificado. */
void create_buffer(t_paquete* paquete); 

#endif