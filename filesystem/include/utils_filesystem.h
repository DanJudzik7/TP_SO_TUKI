#ifndef UTILS_FILESYSTEM_H
#define UTILS_FILESYSTEM_H

#include <dirent.h>
#include <fcntl.h>
#include <math.h>
#include <sys/mman.h>

#include "shared_socket.h"
#include "shared_utils.h"

typedef struct config_filesystem {
	t_log* logger;
	t_config* config;
	t_bitarray* bitmap;
	char* block_file;
	int socket_memory;
	int block_size;
	int block_count;
	char* PATH_SUPERBLOQUE;
	char* PATH_BITMAP;
	char* PATH_BLOQUES;
	char* PATH_FCB;
	int RETARDO_ACCESO;
} config_filesystem;

void setup_config(bool useMemory);

t_config* start_superblock();

t_bitarray* start_bitmap();

char* start_block_file();

#endif