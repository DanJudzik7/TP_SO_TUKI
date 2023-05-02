#ifndef HANDLER_INSTRUCTION_CONSOLA_H
#define HANDLER_INSTRUCTION_CONSOLA_H

#include <stdio.h>
#include <string.h>
#include "shared_utils.h"

// Lee un archivo y manda sus instrucciones al kernel
void process_instructions_from_file(int socket_kernel);

t_package* package_instruction_build(char* lineanstruccion);

op_code return_opcode(char* code);

int string_array_length(char** array);

op_code_reception opcode_receive(char* code) ;

#endif