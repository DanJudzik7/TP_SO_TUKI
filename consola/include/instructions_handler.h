#ifndef INSTRUCTIONS_HANDLER_H
#define INSTRUCTIONS_HANDLER_H

#include "shared_utils.h"
#include "shared_package.h"

// Lee un archivo y manda sus instrucciones al kernel
t_package* process_instructions(int socket_kernel);

// Convierte una instrucción en texto a un package
t_package* parse_instruction(char* instruction);

// Convierte un código de operación a su representación numérica
op_code parse_op_code(char* code);

#endif