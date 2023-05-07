#ifndef SHARED_UTILS_H
#define SHARED_UTILS_H

#include <commons/collections/list.h>
#include <commons/collections/queue.h>
#include <commons/config.h>
#include <commons/log.h>
#include <commons/string.h>
#include <netdb.h>
#include <readline/readline.h>
#include <semaphore.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <time.h>
#include <unistd.h>

typedef enum op_code {
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

typedef struct segment_table {
	int id;
	void* segment_table_direction;
	uint8_t size_data_segment;
} segment_table;

typedef struct cpu_register_4 {
	char AX[4];
	char BX[4];
	char CX[4];
	char DX[4];
} cpu_register_4;

typedef struct cpu_register_8 {
	char EAX[8];
	char EBX[8];
	char ECX[8];
	char EDX[8];
} cpu_register_8;

typedef struct cpu_register_16 {
	char RAX[16];
	char RBX[16];
	char RCX[16];
	char RDX[16];
} cpu_register_16;
typedef struct cpu_register {
	cpu_register_4 register_4;
	cpu_register_8 register_8;
	cpu_register_16 register_16;
} cpu_register;

typedef struct file {
	uint32_t size_file;
	void* file_direction;
} file;

typedef enum process_state {
	NEW,
	READY,
	EXEC,
	BLOCK,
	EXIT_PROCESS
} process_state;

typedef struct execution_context {
	t_queue* instructions;
	int program_counter;
	process_state updated_state;
	cpu_register* cpu_register;
	segment_table* segment_table;
} execution_context;

typedef struct t_pcb {
	int pid; // También funciona como ID del socket
	process_state state;
	int aprox_burst_time;
	time_t last_ready_time;
	t_list* files;
	execution_context* execution_context;
} t_pcb;

typedef enum op_code_reception {
	ERROR,
	OK
} op_code_reception;

// To do: Mover esto a un archivo específico de Kernel
typedef struct t_global_config_kernel {
	t_log* logger;
	t_queue* new_pcbs;
	t_queue* active_pcbs;
	bool algorithm_is_hrrn;
	int max_multiprogramming;
	int default_burst_time;
	int connection_kernel;
} t_global_config_kernel;

typedef struct config_cpu{
    t_log* logger;
    sem_t flag_dislodge;
	sem_t flag_running;
} configuration_cpu;

typedef struct t_buffer {
	uint32_t size;
	void* stream;
} t_buffer;

typedef struct t_package {
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

// Safe Memory Allocation. Crashea si no hay más memoria.
void* s_malloc(size_t size);

/* Envía un paquete a un cliente a través del socket especificado. */
int socket_send_package(t_package* package, int target_socket);

/* Cierra la conexión del socket especificado. */
void socket_close(int target_socket);

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