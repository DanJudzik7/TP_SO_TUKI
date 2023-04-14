#ifndef CONSOLA_H
#define CONSOLA_H

#include <stdio.h>
#include <commons/log.h>
#include <stdbool.h>
#include "shared_utils.h"
#include "tests.h"
#include "socket_utils_consola.h"
#include "serialize_utils_consola.h"
#include <readline/readline.h>

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
} t_instruccion_package2;
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


void list_instruccions(t_list * instruccions);
void process_instruccions(t_list * instruccions);
int count_spaces(char* instruction_text);

#endif


