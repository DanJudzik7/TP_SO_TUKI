#ifndef KERNEL_H
#define KERNEL_H
#include <stdio.h>
#include <commons/log.h>
#include <stdbool.h>
#include "shared_utils.h"
#include "socket_utils_kernel.h"
#include "instruction_handler.h"
#include "tests.h"



t_instruccion_package_0* deserializar_instruction_0(t_buffer* buffer);


#endif