#ifndef FILESYSTEM_H
#define FILESYSTEM_H

#include "shared_socket.h"
#include "shared_utils.h"
#include "shared_package.h"
#include <commons/collections/queue.h>
#include <commons/bitarray.h>
#include <commons/config.h>
#include "commons/log.h"
#include <sys/mman.h>
#include <dirent.h>
#include <fcntl.h>
#include "handler_instruction_filesystem.h"

extern configuration_filesystem config_fs;

typedef struct {
    char *file_name;
    int file_size;
    uint32_t direct_pointer;
    uint32_t indirect_pointer;
} fcb;

// Carga la configuración de un módulo
t_config* start_superblock();
t_bitarray* start_bitmap();
char* start_block_file();

#endif