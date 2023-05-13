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
	F_READ,			 // 0
	F_WRITE,		 // 1
	SET,			 // 2
	MOV_IN,			 // 3
	MOV_OUT,		 // 4
	F_TRUNCATE,		 // 5
	F_SEEK,			 // 6
	CREATE_SEGMENT,	 // 7
	I_O,			 // 8
	WAIT,			 // 9
	SIGNAL,			 // 10
	F_OPEN,			 // 11
	F_CLOSE,		 // 12
	DELETE_SEGMENT,	 // 13
	EXIT,			 // 14
	YIELD			 // 15
} op_code;



typedef struct segment_table {
	uint32_t id;
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

typedef struct t_file {
	uint32_t size_file;
	void* file_direction;
} t_file;

typedef enum process_state {
	NEW,
	READY,
	EXEC,
	BLOCK,
	EXIT_PROCESS
} process_state;

typedef struct execution_context {
	t_queue* instructions;
	uint32_t program_counter;
	process_state updated_state;
	cpu_register* cpu_register;
	segment_table* segment_table;
} execution_context;

typedef enum t_package_type {
	SERIALIZED, // 0
	MESSAGE_OK, // 1
	MESSAGE_FLAW, // 2
	MESSAGE_BUSY, // 3
	INSTRUCTIONS, // 4
	EXECUTION_CONTEXT, // 5
} t_package_type;

typedef enum execution_context_index {
	EC_INSTRUCTIONS,
	PROGRAM_COUNTER,
	UPDATED_STATE,
	CPU_REGISTER,
	SEGMENT_TABLE
} execution_context_index;

typedef struct t_pcb {
	int pid; // También funciona como ID del socket
	process_state state;
	int aprox_burst_time;
	time_t last_ready_time;
	t_list* files;
	execution_context* execution_context;
} t_pcb;

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
	int connection_kernel;
} configuration_cpu;

typedef struct config_memory{
    t_log* logger;
    t_config* config;
	char* algorithm;
} configuration_memory;

typedef struct t_buffer { // Esto se va a ir
	uint32_t size;
	void* stream;
} t_buffer;
typedef struct t_instruction {
	op_code op_code;
	t_list* args;
} t_instruction;

typedef struct t_package {
	uint64_t size;
	int32_t field;
	void* buffer;
} t_package;

typedef struct t_package_reception { // Esto se va a ir
	t_package_type package_type;
	t_buffer* buffer;
} t_package_reception;

// Carga la configuración de un módulo
t_config* start_config(char* module);

// Crea un logger para un módulo
t_log* start_logger(char* module);

// Retorna el valor de una key de un archivo de configuración
char* get_config_type(char* module, char* file_type);

// Inicializa y establece la conexión con un servidor en una dirección IP y puerto específicos. Retorna el descriptor del socket creado.
int socket_initialize(char* ip, char* port);

// Espera y acepta la conexión entrante de un cliente. Retorna el descriptor del socket del cliente conectado.
int socket_accept(int server_socket);

// Inicializa un socket servidor y lo vincula a un puerto específico. Retorna el descriptor del socket creado.
int socket_initialize_server(char* port);

// Recibe un número Unsigned Long de un cliente a través del socket especificado, o NULL si hubo error
uint64_t* socket_receive_long(int target_socket);

// Recibe un número Int de un cliente a través del socket especificado, o NULL si hubo error
int32_t* socket_receive_int(int target_socket);

// Recibe un paquete completo de un cliente a través del socket especificado.
t_package* socket_receive(int target_socket);

// Envía un mensaje a un cliente a través del socket especificado.
bool socket_send_message(int target_socket, char* message, bool error);

// Safe Memory Allocation. Crashea si no hay más memoria.
void* s_malloc(size_t size);

// Envía un paquete a un cliente a través del socket especificado.
bool socket_send(int target_socket, t_package* package);

// Cierra la conexión del socket especificado.
void socket_close(int target_socket);

// Serializa el Execution Context
t_package* serialize_execution_context(execution_context* ec);

// Recibe un execution context
execution_context* deserialize_execution_context(t_package* package);

// Crea un paquete de modo key-value
t_package* package_new_dict(int32_t key, void* value, uint64_t value_size);

// Inserta un paquete dentro de otro
void package_nest(t_package* package, t_package* nested);

// Agrega texto plano a un paquete
void package_write(t_package* package, char* string);

// Crea y retorna un paquete con el código de operación especificado.
t_package* package_new(int32_t field);

// Destruye el paquete especificado
void package_destroy(t_package* package);

// Agrega un valor con un tamaño específico al paquete especificado
void package_add(t_package* package, void* value, uint64_t* value_size);

// Serializa el paquete especificado reemplazando al original
void package_close(t_package* package);

// Devuelve el mensaje deserializado, y destruye el paquete
char* deserialize_message(t_package* package);

// Deserializa una instrucción y la agrega a la queue
void deserialize_instructions(t_package* package, t_queue* instructions);

// En base a las configuraciones, se conecta a un módulo
int connect_module(t_config* config, t_log* logger, char* modulo);

// En desarrollo
int receive_modules(t_log* logger, t_config* config);

// Convierte un código de operación a su representación numérica
op_code get_opcode(char* code);

#endif