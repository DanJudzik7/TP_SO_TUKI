#ifndef SHARED_SOCKET_H
#define SHARED_SOCKET_H

#include <netdb.h>
#include <sys/socket.h>

#include "shared_package.h"
#include "shared_utils.h"

// Librería de socket
// Se encarga de la comunicación de bajo nivel entre procesos a través de sockets

// Inicializa y establece la conexión con un servidor en una dirección IP y puerto específicos. Devuelve el descriptor del socket creado.
int socket_initialize(char* ip, char* port);

// Inicializa un socket servidor y lo vincula a un puerto específico. Devuelve el descriptor del socket creado.
int socket_initialize_server(char* port);

// Espera y acepta la conexión entrante de un cliente en un server. Devuelve el descriptor del socket del cliente conectado.
int socket_accept(int server_socket);

// Envía un paquete a través del socket especificado.
bool socket_send(int target_socket, t_package* package);

// Recibe un paquete completo a través del socket especificado.
t_package* socket_receive(int target_socket);

// Cierra la conexión del socket especificado.
void socket_close(int target_socket);

// En base a las configuraciones, se conecta a un módulo
int connect_module(t_config* config, t_log* logger, char* modulo);

// En desarrollo
int receive_module(t_log* logger, t_config* config);

#endif