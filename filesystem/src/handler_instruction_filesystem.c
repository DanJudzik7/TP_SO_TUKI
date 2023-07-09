#include "handler_instruction_filesystem.h"

int instruction_handler_filesystem() {
    log_error(config_fs.logger, "entro");

    /*while (1) {
		t_package* package = socket_receive(config_fs.connection_kernel);
		if (package == NULL) {
			printf("El cliente se desconectó\n");
			break;
		}*/

        //t_instruction* instruction = s_malloc(sizeof(t_instruction));
        //deserialize_single_instruction(package, instruction);
        t_instruction* instruction = malloc(sizeof(t_instruction));
        instruction->op_code = F_OPEN;
        //instruction->op_code = F_CLOSE;

        instruction->args = list_create();
        char* argument = strdup("prueba");
        list_add(instruction->args, argument);
        printf("El código de operación es: %i\n", instruction->op_code);

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
        //free(package);
        free(instruction);
    //}
    return 0;
}

void truncate_file(t_instruction* instruction){
    if (list_size(instruction->args) < 1) {
        log_error(config_fs.logger, "Instrucción sin argumentos, se esperaba al menos uno");
        abort();
    }
    char* file_name = list_get(instruction->args, 0);
    int* file_size = list_get(instruction->args, 1);
    char* directorio = getcwd(NULL, 0);
	char* full_file_path = string_from_format("%s/cfg/%s%s.dat", directorio,config_fs.PATH_FCB, file_name);
    t_config* fcb_data = config_create(full_file_path);
    if (fcb_data == NULL) {
        open_file(instruction);
        truncate_file(instruction);
    }else{
        clear_bit_position(fcb_data);
        set_bit_position(fcb_data,file_size);
    }
    config_destroy(fcb_data);
    free(full_file_path); 
}
void set_bit_position(t_config* fcb_data,  int* file_size){
    int pd_position=next_bit_position();
    config_set_value(fcb_data, "PUNTERO_DIRECTO", pd_position); 
    if(file_size>config_fs.block_size){
        int pi_position;
        int pi_position_initial;
        pi_position_initial=next_bit_position();
        if(pi_position_initial==-1){
            log_error(config_fs.logger, "No hay mas espacio en filesystem.");
            abort();
        }
        int count_pi = (ceil((double)(*file_size) / config_fs.block_size))-1;
        char* pi_string = malloc(config_fs.block_size * sizeof(char));
        char* end_of_string = pi_string;  // Puntero al final de la cadena.

        for(int i=0; i<count_pi; i++) {
            int pi_position = next_bit_position();
            if(pi_position == -1) {
                log_error(config_fs.logger, "No hay mas espacio en filesystem.");
                abort();
            }
            end_of_string += sprintf(end_of_string, "%d|", pi_position);
        }
        if (count_pi > 0) {
            end_of_string[-1] = '\0'; 
        }
        if (pi_position_initial + strlen(pi_string) < config_fs.block_size * config_fs.block_count) {
            memcpy(config_fs.block_file + pi_position_initial, pi_string, strlen(pi_string));
        } else {
            printf("Error: intentando escribir más allá del final del block_file.\n");
        }
        config_set_value(fcb_data, "PUNTERO_INDIRECTO", pi_position_initial);
    }else{
        config_set_value(fcb_data, "PUNTERO_INDIRECTO", "");
    }
}
void clear_bit_position(t_config* fcb_data){
    int PUNTERO_DIRECTO = config_get_int_value(fcb_data, "PUNTERO_DIRECTO");
    config_fs.block_file[PUNTERO_DIRECTO] = strdup("");
    int PUNTERO_INDIRECTO = config_get_int_value(fcb_data, "PUNTERO_INDIRECTO");
    if(PUNTERO_INDIRECTO != 0){
        t_list* list_punteros = list_create();
        char* punteros_indirectos = config_fs.block_file[PUNTERO_INDIRECTO];
        char** pi_array = string_split(punteros_indirectos, "|");
        for (int i = pi_array[i]; i<(pi_array[i]+config_fs.block_count) ; i++) {
            if(i==pi_array[i]){
                bitarray_clean_bit(config_fs.bitmap, i);
            }
            config_fs.block_file[i] = '\0';
        }
        bitarray_clean_bit(config_fs.bitmap, PUNTERO_INDIRECTO);
        free(pi_array);
    }
    bitarray_clean_bit(config_fs.bitmap, PUNTERO_DIRECTO);
}

void open_file(t_instruction* instruction) {
    if (list_size(instruction->args) < 1) {
        log_error(config_fs.logger, "Instrucción sin argumentos, se esperaba al menos uno");
        abort();
    }
    char* file_name = list_get(instruction->args, 0);


    char* directorio = getcwd(NULL, 0);
	char* full_file_path = string_from_format("%s/cfg/%s%s.dat", directorio,config_fs.PATH_FCB, file_name);

    FILE* fcb = fopen(full_file_path, "r"); 
    if (fcb == NULL) {
        fopen(full_file_path, "w"); 
        log_warning(config_fs.logger, "NO existia el archivo");
        create_file(full_file_path, file_name);
    } else {
        log_warning(config_fs.logger, "Existia el archivo");
        fclose(fcb); 
    }
    config_fs.block_file[0] = 'a';
    config_fs.block_file[1] = 'b';
    config_fs.block_file[2] = 'c';
    for(int i = 0; i < 3; i++) {
        // Leer la posición i
        char c = config_fs.block_file[i];
        printf("Valor en la posición %d: %c\n", i, c);
    }

    free(full_file_path); 
}
void create_file(char* full_file_path, char* file_name) {
    t_config* fcb_data = config_create(full_file_path);
    if(fcb_data==NULL){
        log_warning(config_fs.logger, "Error reando FCB");
        abort();
    }
    config_set_value(fcb_data, "NOMBRE_ARCHIVO", file_name);
    config_set_value(fcb_data, "TAMANIO_ARCHIVO", ""); 
    config_set_value(fcb_data, "PUNTERO_DIRECTO", ""); 
    config_set_value(fcb_data, "PUNTERO_INDIRECTO", ""); 
    log_warning(config_fs.logger, "Creo FCB");

    config_save(fcb_data);
    config_destroy(fcb_data);
}

void close_file(t_instruction* instruction) {
    char* file_name = list_get(instruction->args, 0);
    char* directorio = getcwd(NULL, 0);
	char* full_file_path = string_from_format("%s/cfg/%s%s.dat", directorio,config_fs.PATH_FCB, file_name);
    t_config* fcb_data = config_create(full_file_path);
    clear_bit_position(fcb_data);

    int remove_status = remove(full_file_path);
    if (remove_status == 0) {
        log_info(config_fs.logger, "El archivo %s ha sido cerrado exitosamente", full_file_path);
    } else {
        // Si hay un error, registrar en el log. Esto podría ser porque el archivo no existía.
        log_error(config_fs.logger, "Error al intentar cerrar el archivo %s", full_file_path);
        abort();
    }
    config_destroy(fcb_data);
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
