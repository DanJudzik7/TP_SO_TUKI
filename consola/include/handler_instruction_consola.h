#ifndef HANDLER_INSTRUCTION_CONSOLA_H
#define HANDLER_INSTRUCTION_CONSOLA_H

#include <stdio.h>
#include <string.h>
#include "shared_utils.h"



void list_instruccions(t_list * instruccions);
void process_instruccions(t_list * instruccions,int conexion_kernel);
int count_spaces(char* instruction_text);
t_paquete* handler_instruction_package( char* lineanstruccion);
op_code return_opcode(char* code);
int string_array_length(char** array);
op_code_reception opcode_recive(char* code) ;
#endif


