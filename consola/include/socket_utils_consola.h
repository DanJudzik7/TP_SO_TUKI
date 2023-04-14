#ifndef SOCKET_UTILS_CONSOLA_H
#define SOCKET_UTILS_CONSOLA_H

#include "serialize_utils_consola.h"
#include "shared_utils.h"


int socket_initialize_connect(char *ip,char *puerto);

void socket_send_message(char* mensaje, int socket_cliente);

void socket_send_package(t_paquete* paquete, int socket_cliente);

void delete_package(t_paquete* paquete);

void socket_end(int socket_cliente);


#endif