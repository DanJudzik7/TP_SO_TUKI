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


void list_instruccions(t_list * instruccions);
void process_instruccions(t_list * instruccions,int conexion_kernel);
int count_spaces(char* instruction_text);

#endif


