#include "filesystem.h"

configuration_filesystem config_fs;

int main(int argc, char** argv) {
    config_fs.logger = start_logger("filesystem");
	config_fs.config = start_config("filesystem");
	log_warning(config_fs.logger, "Iniciando el filesystem");

	char* port = config_get_string_value(config_fs.config, "PUERTO_ESCUCHA");	
    config_fs.PATH_SUPERBLOQUE = config_get_string_value(config_fs.config, "PATH_SUPERBLOQUE");
    config_fs.PATH_BITMAP = config_get_string_value(config_fs.config, "PATH_BITMAP");
    config_fs.PATH_BLOQUES = config_get_string_value(config_fs.config, "PATH_BLOQUES");
    config_fs.PATH_FCB = config_get_string_value(config_fs.config, "PATH_FCB");
	
    int socket_fs = socket_initialize_server(port); // Inicializo el socket en el puerto cargado por la config
	log_warning(config_fs.logger, "Socket %d de servidor inicializado en puerto %s", socket_fs, port);
    
	t_config* superblock = start_superblock();
	config_fs.block_size = config_get_int_value(superblock, "BLOCK_SIZE");
	int block_count = config_get_int_value(superblock, "BLOCK_COUNT");

	log_warning(config_fs.logger, "Tamaño de bloque: %d, Cantidad de bloques: %d", config_fs.block_size, block_count);

	config_fs.bitmap = start_bitmap(block_count);
    config_fs.block_file = start_block_file(block_count);

    config_fs.connection_kernel = socket_accept(socket_fs);
    instruction_handler_filesystem();

}


t_config* start_superblock() {
    log_warning(config_fs.logger, "ruta: %s",config_fs.PATH_SUPERBLOQUE);
    char* full_path_superblock = get_full_path(config_fs.PATH_SUPERBLOQUE);
    t_config* superblock = config_create(full_path_superblock);
    if (superblock == NULL) {
        log_warning(config_fs.logger, "Error al cargar el archivo de superbloque");
        abort();
    }
    free(full_path_superblock);
    return superblock;
}

t_bitarray* start_bitmap(int block_count) {
    int bitmap_file = open(config_fs.PATH_BITMAP, O_RDWR | O_CREAT, 0664);
    if(bitmap_file < 0) {
        log_warning(config_fs.logger, "Error al abrir el archivo de bitmap");
        abort();
    }
    int bitmap_size = block_count / 8;
    if (block_count % 8 != 0) bitmap_size++;
    ftruncate(bitmap_file, bitmap_size); // Asegura que el tamaño del archivo es correcto
    char* bitmap_data = mmap(NULL, bitmap_size, PROT_WRITE | PROT_READ, MAP_SHARED, bitmap_file, 0);
    if (bitmap_data == MAP_FAILED) {
        log_warning(config_fs.logger, "Error al mapear el archivo de bitmap");
        abort();
    }
    t_bitarray* bitmap = bitarray_create(bitmap_data, bitmap_size);
    if (bitmap == NULL) {
        log_warning(config_fs.logger, "Error al cargar el bitmap");
        abort();
    }

    return bitmap;
}

char* start_block_file(int block_count) {
    int fd = open(config_fs.PATH_BLOQUES, O_RDWR | O_CREAT, 0664);
    if (fd < 0) {
        log_warning(config_fs.logger, "Error al abrir el archivo de bloques");
        abort();
    }
    if (ftruncate(fd, block_count * (config_fs.block_size)) == -1) {
        log_warning(config_fs.logger, "Error al ajustar el tamaño del archivo de bloques");
        abort();
    }
    char* block_file = mmap(NULL, block_count * (config_fs.block_size), PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0);
    if (block_file == MAP_FAILED) {
        log_warning(config_fs.logger, "Error al mapear el archivo de bloques");
        abort();
    }

    return block_file;
}