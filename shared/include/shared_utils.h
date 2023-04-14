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

typedef struct
{
	uint32_t size;
	void* stream;
} t_buffer;

typedef struct {
    uint32_t instruccion_long;
    char* instruccion;
    uint32_t size_param_1;
    char* param_1;
    uint32_t size_param_2;
    char* param_2;
    uint32_t size_param_3;
    char* param_3;
} t_instruccion_package_3;
typedef struct {
    uint32_t instruccion_long;
    char* instruccion;
    uint32_t size_param_1;
    char* param_1;
    uint32_t size_param_2;
    char* param_2;
} t_instruccion_package_2;
typedef struct {
    uint32_t instruccion_long;
    char* instruccion;
    uint32_t size_param_1;
    char* param_1;
} t_instruccion_package_1;
typedef struct {
    uint32_t instruccion_long;
    char* instruccion;
} t_instruccion_package_0;

char* mi_funcion_compartida();
char* obtener_cfg_type(char* process_name,char* file_type);
t_config* iniciar_config(char* process_name);
t_log* iniciar_logger(char* process_name);

#endif