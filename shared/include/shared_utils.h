#ifndef SHARED_UTILS_H
#define SHARED_UTILS_H

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <time.h>
#include <commons/config.h>
#include <commons/collections/list.h>
#include <commons/collections/queue.h>
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
//REVISAR
typedef struct{
	int id;
	void* segment_table_direction;
	uint8_t size_data_segment;
}segment_table;
//TODO
typedef struct{
	int acumulator;
	int register_base;
	int counter;
	int register_data;  
} cpu_register;

typedef struct{
	uint32_t size_file;
	void* file_direction;
} file;

typedef enum{
	NEW,
	READY,
	EXEC,
	BLOCK,
	EXIT_PROCESS
} state_pcb;

/* Comento y despues vemos que version sirve
typedef struct {
    int pid;
	state_pcb state_pcb;
    op_code* instructions; //RECORDAR QUE DEBE HACERSE puntero a opcode
    int program_counter;
    cpu_register cpu_register;
    segment_table segment_table;
    int aprox_burst_time;
    time_t last_ready_time;
    file* table_open_files;
} pcb;
*/
typedef struct{
	int pid;
	t_list* instruccions;
	int program_counter;
	cpu_register cpu_register;
	segment_table segment_table;
}execution_context;

typedef struct{
	execution_context execution_context;
	int aprox_burst_time;
    time_t last_ready_time;
    file* table_open_files;
	state_pcb state_pcb;
} pcb;
typedef enum{
	ERROR,
	OK
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

//TODO IMPLEMTENAR PARA DESERIALIZAR EL PAYLOAD, CAPAZ ESTO DEPENDA DEL TIPO QUE NECESITEMOS ESTILO INT O CHAR*
char* deserialize_payload(t_list* payload);

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
int socket_recv_message(int socket_cliente); 

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

/* Libera la memoria asignada a un paquete especificado. */
void delete_package(t_paquete* paquete); 

/* Crea un buffer para el paquete especificado. */
void create_buffer(t_paquete* paquete); 

t_list* socket_recv_package(int socket_cliente);

int conect_modules(t_config* config,t_log* logger,char* modulo);

int receive_modules(t_log* logger,t_config* config);
#endif