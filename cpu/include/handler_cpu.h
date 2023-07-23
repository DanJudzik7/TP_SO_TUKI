#ifndef HANDLER_CPU_H
#define HANDLER_CPU_H

#include <math.h>
#include <unistd.h>

#include "shared_serializer.h"
#include "shared_socket.h"
#include "utils_cpu.h"

extern configuration_cpu config_cpu;

// Obtiene la próxima instrucción a ejecutar
t_instruction* fetch(t_execution_context* execution_context);

// Decodifica la instrucción. Devuelve la dirección física asociada.
t_physical_address* decode(t_instruction* instruction, t_execution_context* ec);

// Ejecuta la instrucción. Devuelve True si debe desalojar.
void execute(t_instruction* instruction, t_execution_context* ec, t_physical_address* associated_pa);

// Establece el valor de un registro
void set_register(char* register_name, char* value, t_registers* registers);

#endif