#ifndef FILESYSTEM_H
#define FILESYSTEM_H

#include <pthread.h>

#include "handler_instruction_filesystem.h"
#include "shared_socket.h"
#include "shared_utils.h"
#include "utils_filesystem.h"

extern config_filesystem config_fs;

typedef struct {
	char* file_name;
	int file_size;
	uint32_t direct_pointer;
	uint32_t indirect_pointer;
} fcb;

#endif