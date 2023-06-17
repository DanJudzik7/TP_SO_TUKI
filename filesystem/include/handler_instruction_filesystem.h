#ifndef HANDLER_INSTRUCTION_FILESYSTEM_H
#define HANDLER_INSTRUCTION_FILESYSTEM_H

#include "shared_serializer.h"
#include "shared_socket.h"

int instruction_handler_filesystem(configuration_filesystem *config_fs);
void open_file(configuration_filesystem* config_fs, t_instruction* instruction);
void close_file(configuration_filesystem* config_fs, t_instruction* instruction);

#endif