#ifndef SHARED_UTILS_H
#define SHARED_UTILS_H

#include <commons/collections/list.h>
#include <commons/collections/queue.h>
#include <commons/config.h>
#include <commons/log.h>
#include <commons/string.h>
#include <netdb.h>
#include <readline/readline.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <time.h>
#include <semaphore.h>
#include <unistd.h>

typedef enum {
	MENSAJE,		 // 0     //Este existe de mas
	F_READ,			 // 1
	F_WRITE,		 // 2
	SET,			 // 3
	MOV_IN,			 // 4
	MOV_OUT,		 // 5
	F_TRUNCATE,		 // 6
	F_SEEK,			 // 7
	CREATE_SEGMENT,	 // 8
	I_O,			 // 9
	WAIT,			 // 10
	SIGNAL,			 // 11
	F_OPEN,			 // 12
	F_CLOSE,		 // 13
	DELETE_SEGMENT,	 // 14
	EXIT,			 // 15
	YIELD			 // 16
} op_code;

typedef struct {
	int id;
	void* segment_table_direction;
	uint8_t size_data_segment;
} segment_table;
// TODO
typedef struct {
	char AX[4];
	char BX[4];
	char CX[4];
	char DX[4];
} cpu_register_4;

typedef struct {
	char EAX[8];
	char EBX[8];
	char ECX[8];
	char EDX[8];
} cpu_register_8;

typedef struct {
	char RAX[16];
	char RBX[16];
	char RCX[16];
	char RDX[16];
} cpu_register_16;
typedef struct {
	cpu_register_4 register_4;
	cpu_register_8 register_8;
	cpu_register_16 register_16;
} cpu_register;

typedef struct {
	uint32_t size_file;
	void* file_direction;
} file;

typedef enum {
	NEW,
	READY,
	EXEC,
	BLOCK,
	EXIT_PROCESS
} state_pcb;

typedef struct {
	t_queue* instructions;
	int program_counter;
	cpu_register cpu_register;
	segment_table segment_table;
	state_pcb state_pcb;
} execution_context;

typedef struct {
	int pid;
	execution_context* execution_context;
	int aprox_burst_time;
	time_t last_ready_time;
	file* table_open_files;
	state_pcb state_pcb;
} t_pcb;

typedef enum {
	ERROR,
	OK
} op_code_reception;

typedef struct {
	t_log* logger;
	int console;
	t_queue* global_pcb;
	char* algorithm;
	int max_multiprogramming;
	int default_burst_time;
	int conection_kernel;
	int conection_module_cpu;
	int conection_module_memory;
	int conection_module_filesystem;
} global_config_kernel;

typedef struct{
	int conection_module_console;
	t_pcb* pcb;
} process;

typedef struct 
{	
	global_config_kernel* global_config_kernel;
	process* current_process;
} config_current_process ;


typedef struct {
	uint32_t size;
	void* stream;
} t_buffer;

typedef struct {
	op_code op_code;
	t_buffer* buffer;
} t_package;

char* get_config_type(char* process_name, char* file_type);
t_config* start_config(char* process_name);
t_log* start_logger(char* process_name);

// TODO IMPLEMENTAR PARA DESERIALIZAR EL PAYLOAD, CAPAZ ESTO DEPENDA DEL TIPO QUE NECESITEMOS ESTILO INT O CHAR*
char* deserialize_payload(t_list* payload);

/* Inicializa y establece la conexión con un servidor en una dirección IP y puerto específicos. Retorna el descriptor del socket creado. */
int socket_initialize(char* ip, char* port);

/* Espera y acepta la conexión entrante de un cliente. Retorna el descriptor del socket del cliente conectado. */
int socket_accept(int server_socket);

/* Inicializa un socket servidor y lo vincula a un puerto específico. Retorna el descriptor del socket creado. */
int socket_initialize_server(char* port);

/* Recibe y retorna el código de operación enviado por un cliente a través del socket especificado. */
int socket_receive_operation(int target_socket);

/* Envía un mensaje a un cliente a través del socket especificado. */
int socket_send_message(char* message, int target_socket);

/* Recibe y muestra un mensaje enviado por un cliente a través del socket especificado. */
int socket_receive_message(int target_socket);

/* Envía un paquete a un cliente a través del socket especificado. */
int socket_send_package(t_package* package, int target_socket);

/* Cierra la conexión del socket especificado. */
void socket_end(int target_socket);

/* Crea y retorna un paquete con el código de operación especificado. */
t_package* package_create(int cod_op);

/* Agrega un valor con un tamaño específico al paquete especificado. */
void package_add(t_package* package, void* value, int size);

/* Serializa el paquete especificado en un buffer de bytes de tamaño específico y retorna un puntero al buffer. */
void* package_serialize(t_package* package, int bytes);

/* Libera la memoria asignada a un paquete especificado. */
void package_delete(t_package* package);

t_list* socket_receive_package(int target_socket);

int connect_module(t_config* config, t_log* logger, char* modulo);

int receive_modules(t_log* logger, t_config* config);

op_code return_opcode(char* code);

#endif