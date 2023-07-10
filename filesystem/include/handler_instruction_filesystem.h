#ifndef HANDLER_INSTRUCTION_FILESYSTEM_H
#define HANDLER_INSTRUCTION_FILESYSTEM_H

#include "shared_serializer.h"
#include "shared_socket.h"
#include <commons/config.h>
#include <math.h>

extern configuration_filesystem config_fs;

int instruction_handler_filesystem();
int next_bit_position();
void open_file(t_instruction* instruction);
void close_file(t_instruction* instruction);
void create_file(char* full_file_path, char* file_name);
void clear_bit_position(t_config* fcb_data);
void set_bit_position(t_config* fcb_data, int* file_size);

#endif