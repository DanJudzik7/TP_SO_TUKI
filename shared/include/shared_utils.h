#ifndef SHARED_UTILS_H
#define SHARED_UTILS_H

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <commons/config.h>
#include <commons/log.h>
#include <stdbool.h>
#include <netdb.h>
#include <sys/socket.h>

char* mi_funcion_compartida();
char* obtener_cfg_type(char* process_name,char* file_type);
t_config* iniciar_config(char* process_name);
t_log* iniciar_logger(char* process_name);


#endif