#include "handler_instruction_filesystem.h"

int instruction_handler_filesystem() {
	// Todo esto no está adaptado al nuevo packaging
	while (1) {
		t_package* package = socket_receive(config_fs.connection_kernel);
		if (package == NULL) {
			printf("El cliente se desconectó\n");
			break;
		}
        t_instruction* instruction = s_malloc(sizeof(t_instruction));
        deserialize_single_instruction(package, instruction);
		printf("El código de operación es: %i\n", instruction->op_code);
        //deserialize_instructions
		switch (instruction->op_code) {
			case F_OPEN:
				printf("RECIBIMOS UNA INSTRUCCIÓN DE ABRIR ARCHIVO\n");
                open_file(instruction);
				if (!socket_send(config_fs.connection_kernel, serialize_message("OK_OPEN_FILE", false))) {
					printf("Error al enviar el paquete\n");
					return -1;
				}
				break;
			case F_CLOSE:
				printf("RECIBIMOS UNA INSTRUCCIÓN DE CERRAR UN ARCHIVO\n");
                close_file(instruction);
				if (!socket_send(config_fs.connection_kernel, serialize_message("OK_CLOSE_FILE", false))) {
					printf("Error al enviar el paquete\n");
					return -1;
				}
				break;
			default:
				printf("Error al recibir código de operación\n");
				return -1;
		}
	}
	return 0;
}
void truncate_file(t_instruction* instruction){
    if (list_size(instruction->args) < 1) {
        log_error(config_fs.logger, "Instrucción sin argumentos, se esperaba al menos uno");
        abort();
    }
    char* file_name = list_get(instruction->args, 0);
    char* file_size = list_get(instruction->args, 1);
    char* full_file_path = malloc(strlen(config_fs.PATH_FCB) + strlen(file_name) + strlen(".dat") + 2);
    sprintf(full_file_path, "Se ve a truncar el archivo: %s/%s.dat", config_fs.PATH_FCB, file_name);
    t_config* fcb_data = config_create(full_file_path);
    if (fcb_data == NULL) {
        open_file(instruction);
        truncate_file(instruction);
    }else{
        if(file_size==0){
            clear_bitmap_position(fcb_data);
        }else{
            int bitmap_position = next_bit_position();
            config_set_value(fcb_data, "TAMANIO_ARCHIVO", file_size); 
            config_set_value(fcb_data, "PUNTERO_DIRECTO", bitmap_position); 
            free(bitmap_position); 
            if(file_size>config_fs.block_size){
                int bitmap_position_indirect = next_bit_position();
                config_set_value(fcb_data, "PUNTERO_INDIRECTO", bitmap_position_indirect); 
                free(bitmap_position_indirect); 
            }else{
                config_set_value(fcb_data, "PUNTERO_INDIRECTO", ""); 
            }
            config_destroy(fcb_data);
        }
    } 

    free(full_file_path); 
}
void clear_bitmap_position(t_config* fcb_data){
    config_set_value(fcb_data, "TAMANIO_ARCHIVO", "0"); 
    int PUNTERO_DIRECTO = config_get_string_value(fcb_data, "PUNTERO_DIRECTO");
    int PUNTERO_INDIRECTO = config_get_string_value(fcb_data, "PUNTERO_INDIRECTO");
    bitarray_clean_bit(fcb_data, PUNTERO_DIRECTO);
    bitarray_clean_bit(fcb_data, PUNTERO_INDIRECTO);
}

void open_file(t_instruction* instruction) {
    if (list_size(instruction->args) < 1) {
        log_error(config_fs.logger, "Instrucción sin argumentos, se esperaba al menos uno");
        abort();
    }
    char* file_name = list_get(instruction->args, 0);
    char* full_file_path = malloc(strlen(config_fs.PATH_FCB) + strlen(file_name) + strlen(".dat") + 2);
    sprintf(full_file_path, "%s/%s.dat", config_fs.PATH_FCB, file_name);

    FILE* fcb = fopen(full_file_path, "r"); 
    if (fcb == NULL) {
        create_file(full_file_path, file_name);
    } else {
        fclose(fcb); 
    }

    free(full_file_path); 
}

void create_file(char* full_file_path, char* file_name) {
    t_config* fcb_data = config_create(full_file_path);

    config_set_value(fcb_data, "NOMBRE_ARCHIVO", file_name);
    config_set_value(fcb_data, "TAMANIO_ARCHIVO", ""); 
    config_set_value(fcb_data, "PUNTERO_DIRECTO", ""); 
    config_set_value(fcb_data, "PUNTERO_INDIRECTO", ""); 

    config_save(fcb_data);
    config_destroy(fcb_data);
}

void close_file(t_instruction* instruction) {
    char* file_name = list_get(instruction->args, 0);
    char* full_file_path = malloc(strlen(config_fs.PATH_FCB) + strlen(file_name) + strlen(".dat") + 2);
    sprintf(full_file_path, "%s/%s.dat", config_fs.PATH_FCB, file_name);
    
    t_config* fcb_data = config_create(full_file_path);
    clear_bitmap_position(fcb_data);

    int remove_status = remove(full_file_path);
    if (remove_status == 0) {
        log_info(config_fs.logger, "El archivo %s ha sido cerrado exitosamente", full_file_path);
    } else {
        // Si hay un error, registrar en el log. Esto podría ser porque el archivo no existía.
        log_error(config_fs.logger, "Error al intentar cerrar el archivo %s", full_file_path);
        abort();
    }
    
    free(full_file_path);
}
int next_bit_position() {
    int longitud = bitarray_get_max_bit(config_fs.bitmap);

    for(int i = 0; i < longitud; i++) {
        if(!bitarray_test_bit(config_fs.bitmap, i)) {
            bitarray_set_bit(config_fs.bitmap, i);
            return i;
        }
    }
    return -1;
}