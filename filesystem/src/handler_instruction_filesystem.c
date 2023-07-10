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
        //instruction->op_code = F_OPEN;
        instruction->op_code = F_CLOSE;
        //instruction->op_code = F_TRUNCATE;

        instruction->args = list_create();
        char* argument = strdup("prueba2");
        list_add(instruction->args, argument);
        char* argument2 = strdup("32");
        list_add(instruction->args, argument2);
        printf("El código de operación es: %i\n", instruction->op_code);

        switch (instruction->op_code) {
            case F_OPEN:
                printf("RECIBIMOS UNA INSTRUCCIÓN DE ABRIR ARCHIVO\n");
                open_file(instruction);
                for(int i = 0; i < 256; i++) {
                    // Leer la posición i
                    char c = config_fs.block_file[i];
                    printf("Valor en la posición %d: %c\n", i, c);
                }
                if (!socket_send(config_fs.connection_kernel, serialize_message("OK_OPEN_FILE", false))) {
                    printf("Error al enviar el paquete\n");
                    return -1;
                }
                break;
            case F_CLOSE:
                printf("RECIBIMOS UNA INSTRUCCIÓN DE CERRAR UN ARCHIVO\n");
                close_file(instruction);
                for(int i = 0; i < 256; i++) {
                    // Leer la posición i
                    char c = config_fs.block_file[i];
                    printf("Valor en la posición %d: %c\n", i, c);
                }
                if (!socket_send(config_fs.connection_kernel, serialize_message("OK_CLOSE_FILE", false))) {
                    printf("Error al enviar el paquete\n");
                    return -1;
                }
                break;
            case F_TRUNCATE:
                printf("RECIBIMOS UNA INSTRUCCIÓN DE TRUNCAR UN ARCHIVO\n");
                truncate_file(instruction);
                for(int i = 0; i < 256; i++) {
                    // Leer la posición i
                    char c = config_fs.block_file[i];
                    printf("Valor en la posición %d: %c\n", i, c);
                }
                if (!socket_send(config_fs.connection_kernel, serialize_message("OK_TRUNCATE_FILE", false))) {
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
    char* char_file_size = list_get(instruction->args, 1);
    int file_size = atoi(char_file_size); // no necesitas hacer malloc para un int si lo vas a usar inmediatamente
    char* directorio = getcwd(NULL, 0);
	char* full_file_path = string_from_format("%s/cfg/%s%s.dat", directorio,config_fs.PATH_FCB, file_name);
    t_config* fcb_data = config_create(full_file_path);
    if (fcb_data == NULL) {
        open_file(instruction);
        truncate_file(instruction);
    }else{
        char* PUNTERO_DIRECTO = config_get_string_value(fcb_data, "PUNTERO_DIRECTO");
        if(strcmp(PUNTERO_DIRECTO, "") != 0 ){
            clear_bit_position(fcb_data);
        }
        set_bit_position(fcb_data,&file_size);
        free(PUNTERO_DIRECTO); 
    }
    free(directorio); // liberar la memoria del getcwd
    free(full_file_path); 
    config_destroy(fcb_data); // usar config_destroy en lugar de free
    
}

void set_bit_position(t_config* fcb_data,  int* file_size){
    int pd_position=next_bit_position();
    for(int i = 0; i < config_fs.block_count; i++) {
        bool car = bitarray_test_bit(config_fs.bitmap, i);
        printf("BIT:%i %s\n", i, car ? "Verdadero" : "Falso");
    }
    char* pd_position_string = malloc(12 * sizeof(char));
    sprintf(pd_position_string, "%d", pd_position);
    config_set_value(fcb_data, "PUNTERO_DIRECTO", pd_position_string); 
    free(pd_position_string); // liberar la memoria después de usarla
    if(*file_size > config_fs.block_size){
        int pi_position_initial=next_bit_position();
        if(pi_position_initial==-1){
            log_error(config_fs.logger, "No hay mas espacio en filesystem.");
            abort();
        }

        int count_pi = (*file_size + config_fs.block_size - 1) / config_fs.block_size - 1;
        char* pi_string = malloc(config_fs.block_size * sizeof(char));
        char* end_of_string = pi_string;  // Puntero al final de la cadena.
        int pi_position; 
        for(int i=0; i<count_pi; i++) {
            pi_position = next_bit_position();
            if(pi_position == -1) {
                log_error(config_fs.logger, "No hay mas espacio en filesystem.");
                abort();
            }
            end_of_string += sprintf(end_of_string, "%i|", pi_position);
        }
        if (count_pi > 0) {
            end_of_string[-1] = '\0'; 
        }
        if ((pi_position_initial*config_fs.block_size) + strlen(pi_string) < config_fs.block_size * config_fs.block_count) {
            memcpy(config_fs.block_file + (pi_position_initial*config_fs.block_size), pi_string, strlen(pi_string));
        } else {
            printf("Error: intentando escribir más allá del final del block_file.\n");
        }
        free(pi_string); // liberar la memoria después de usarla
        char* pi_position_string = malloc(12 * sizeof(char));
        sprintf(pi_position_string, "%d", pi_position_initial);
        config_set_value(fcb_data, "PUNTERO_INDIRECTO", pi_position_string);
        free(pi_position_string); // liberar la memoria después de usarla
    } else {
        config_set_value(fcb_data, "PUNTERO_INDIRECTO", "");
    }
    config_save(fcb_data);
}


void clear_bit_position(t_config* fcb_data){
    int PUNTERO_DIRECTO = config_get_int_value(fcb_data, "PUNTERO_DIRECTO");
    for(int i = PUNTERO_DIRECTO*config_fs.block_size; i < (PUNTERO_DIRECTO*config_fs.block_size) + config_fs.block_size; i++) {
        config_fs.block_file[i] = 0;
    }
    char* PUNTERO_INDIRECTO_STR = config_get_string_value(fcb_data, "PUNTERO_INDIRECTO");
    if(strcmp(PUNTERO_INDIRECTO_STR, "") != 0 ){
        int PUNTERO_INDIRECTO = atoi(PUNTERO_INDIRECTO_STR);
        char* pi_string = malloc(config_fs.block_size * sizeof(char));
        char* pi_string_ptr = pi_string; 
        for(int i = (PUNTERO_INDIRECTO*config_fs.block_size); i < ((PUNTERO_INDIRECTO*config_fs.block_size)+config_fs.block_size); i++) {
            char c = config_fs.block_file[i];
            pi_string_ptr += sprintf(pi_string_ptr, "%c", c);
        }
        char** pi_array = string_split(pi_string, "|");
        for (int i = 0; pi_array[i] != NULL; i++) {
            int pi_pos = atoi(pi_array[i]);
            bitarray_clean_bit(config_fs.bitmap, pi_pos);
            for(int i = pi_pos*config_fs.block_size; i < (pi_pos*config_fs.block_size) + config_fs.block_size; i++) {
                config_fs.block_file[i] = 0;
            }
            free(pi_array[i]); // liberamos cada string en el array
        }
        for(int i = PUNTERO_INDIRECTO*config_fs.block_size; i < (PUNTERO_INDIRECTO*config_fs.block_size) + config_fs.block_size; i++) {
            config_fs.block_file[i] = 0;
        }
        bitarray_clean_bit(config_fs.bitmap, PUNTERO_INDIRECTO);
        free(pi_array); // liberamos el array de strings
        free(pi_string); // liberamos la cadena original
    }
    bitarray_clean_bit(config_fs.bitmap, PUNTERO_DIRECTO);
    config_set_value(fcb_data, "PUNTERO_DIRECTO", ""); 
    config_set_value(fcb_data, "PUNTERO_INDIRECTO", ""); 
    config_save(fcb_data);
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
    free(full_file_path); 
    free(directorio); // liberamos directorio
}
void create_file(char* full_file_path, char* file_name) {
    t_config* fcb_data = config_create(full_file_path);
    if(fcb_data==NULL){
        log_warning(config_fs.logger, "Error creando FCB"); // liberamos full_file_path antes de abortar
        abort();
    }
    config_set_value(fcb_data, "NOMBRE_ARCHIVO", file_name);
    config_set_value(fcb_data, "TAMANIO_ARCHIVO", ""); 
    config_set_value(fcb_data, "PUNTERO_DIRECTO", ""); 
    config_set_value(fcb_data, "PUNTERO_INDIRECTO", ""); 
    log_warning(config_fs.logger, "Creo FCB");

    config_save(fcb_data);
    config_destroy(fcb_data);// liberamos full_file_path
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
        log_error(config_fs.logger, "Error al intentar cerrar el archivo %s", full_file_path);
        config_destroy(fcb_data); // liberamos fcb_data antes de abortar
        free(full_file_path); // liberamos full_file_path antes de abortar
        abort();
    }
    config_destroy(fcb_data); // liberamos fcb_data
    free(full_file_path); // liberamos full_file_path
    free(directorio); // liberamos directorio
}

int next_bit_position() {
    for(int i = 0; i < config_fs.block_count; i++) {
        bool car = bitarray_test_bit(config_fs.bitmap, i);
        printf("BIT:%i %s\n", i, car ? "Verdadero" : "Falso");
        if(!bitarray_test_bit(config_fs.bitmap, i)) {
            bitarray_set_bit(config_fs.bitmap, i);
            return i;
        }
    }
    return -1;
}
