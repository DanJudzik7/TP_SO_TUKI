#include "filesystem.h"

configuration_filesystem* config_fs;

int main(int argc, char** argv) {
    config_fs->logger = start_logger("filesystem");
	t_config* config = start_config("filesystem");
	log_warning(config_fs->logger, "Iniciando el filesystem");

	char* port = config_get_string_value(config, "PUERTO_ESCUCHA");	
    char* PATH_SUPERBLOQUE = config_get_string_value(config, "PATH_SUPERBLOQUE");
    char* PATH_BITMAP = config_get_string_value(config, "PATH_BITMAP");
    char* PATH_BLOQUES = config_get_string_value(config, "PATH_BLOQUES");
    char* PATH_FCB = config_get_string_value(config, "PATH_FCB"); // Obtenemos el puerto con el que escucharemos conexiones
	
    int socket_fs = socket_initialize_server(port); // Inicializo el socket en el puerto cargado por la config
	log_warning(config_fs->logger, "Socket %d de servidor inicializado en puerto %s", socket_fs, port);
    
	t_config* superblock = start_superblock(PATH_SUPERBLOQUE);
	int block_size = config_get_int_value(superblock, "BLOCK_SIZE");
	int block_count = config_get_int_value(superblock, "BLOCK_COUNT");

	log_warning(config_fs->logger, "Tamaño de bloque: %d, Cantidad de bloques: %d", block_size, block_count);

	t_bitarray* bitmap = start_bitmap(block_count, PATH_BITMAP);
    FILE* block_file = start_block_file(block_count, block_size, PATH_BLOQUES);

    config_fs->connection_kernel = socket_accept(socket_fs);
    instruction_handler_filesystem(config_fs);

    /*t_queue* fcb_queue = queue_create();
    load_all_fcbs(PATH_FCB, fcb_queue);

    int size = queue_size(fcb_queue);
    log_warning(logger, "Tamaño de queqe: %d", size);
    for (int i = 0; i < size; i++) {
        fcb* fcb_next = queue_peek(fcb_queue);
        log_warning(logger, "Nombre de archivo: %s, Tamaño de archivo: %d, Puntero directo: %u, Puntero indirecto: %u", 
                fcb_next->file_name, fcb_next->file_size, fcb_next->direct_pointer, fcb_next->indirect_pointer);
    }*/
}


t_config* start_superblock(char* path_superblock) {
	char* full_path_superblock = get_full_path(path_superblock);
	t_config* superblock = config_create(full_path_superblock);
	if (superblock == NULL) {
		printf("Error al cargar el archivo de superbloque\n");
		abort();
	}
	free(path_superblock);
	return superblock;
}

// Este método inicializa el bitmap
t_bitarray* start_bitmap(int block_count, char* bitmap_file_path) {
    int bitmap_file = open(bitmap_file_path, O_RDWR | O_CREAT, 0664);
    if(bitmap_file < 0) {
        printf("Error al abrir el archivo de bitmap\n");
        abort();
    }
    int bitmap_size = block_count / 8;
    if (block_count % 8 != 0) bitmap_size++;
    ftruncate(bitmap_file, bitmap_size); // Asegura que el tamaño del archivo es correcto
    char* bitmap_data = mmap(NULL, bitmap_size, PROT_WRITE | PROT_READ, MAP_SHARED, bitmap_file, 0);
    if (bitmap_data == MAP_FAILED) {
        printf("Error al mapear el archivo de bitmap\n");
        abort();
    }
    t_bitarray* bitmap = bitarray_create(bitmap_data, bitmap_size);
    if (bitmap == NULL) {
        printf("Error al cargar el bitmap\n");
        abort();
    }

    return bitmap;
}

FILE* start_block_file(int block_count, int block_size,char* path) {
    FILE* block_file = fopen("cfg/blocks.dat", "wb+");
    if (block_file == NULL) {
        printf("Error al abrir el archivo de bloques\n");
        abort();
    }
    fseek(block_file, block_count * block_size - 1, SEEK_SET);
    fputc('\0', block_file);
    return block_file;
}
fcb* load_fcb(char* path) {
    t_config* config = config_create(path);
    fcb* fcb_new = malloc(sizeof(fcb));
    char* file_name_temp = config_get_string_value(config, "NOMBRE_ARCHIVO");
    fcb_new->file_name = strdup(file_name_temp);
    fcb_new->file_size = config_get_int_value(config, "TAMANIO_ARCHIVO");
    fcb_new->direct_pointer = config_get_int_value(config, "PUNTERO_DIRECTO");
    fcb_new->indirect_pointer = config_get_int_value(config, "PUNTERO_INDIRECTO");
    config_destroy(config);
    return fcb_new;
}
void load_all_fcbs(char* fcb_directory, t_queue* fcb_queue) {
    DIR* directory = opendir(fcb_directory);
    struct dirent* file;
    while((file = readdir(directory)) != NULL) {
        if(file->d_type == DT_REG) { // Si el directorio es un archivo
            char* fcb_path = malloc(strlen(fcb_directory) + strlen(file->d_name) + 2);
            sprintf(fcb_path, "%s/%s", fcb_directory, file->d_name);
            fcb* fcb_new = load_fcb(fcb_path);
            // Encolar el FCB
            queue_push(fcb_queue, fcb_new);
            // Recuerda liberar la memoria de fcb_path cuando ya no la necesites
            free(fcb_path);
        }
    }
    closedir(directory);
}
