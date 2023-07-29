#ifndef HANDLER_INSTRUCTION_FILESYSTEM_H
#define HANDLER_INSTRUCTION_FILESYSTEM_H

#include <math.h>

#include "shared_serializer.h"
#include "shared_socket.h"
#include "utils_filesystem.h"

int handle_kernel(int* socket_kernel);

bool process_instruction(t_instruction* instruction);

int next_bit_position();
bool open_file(t_instruction* instruction);
void create_file(char* full_file_path, char* file_name);
void resize_block(t_config* fcb_data, int* file_size, char* file_name);
t_list* get_bf_ip(int PUNTERO_INDIRECTO);
void set_bf_ip(int PUNTERO_INDIRECTO, t_list* pi_list);
void truncate_file(t_instruction* instruction);
char* iterate_block_file(t_instruction* instruction);

#endif