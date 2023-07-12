#include "handler_instruction_filesystem.h"

int instruction_handler_filesystem() {
    log_error(config_fs.logger, "entro");

    /*t_list* numbers = list_create();
    int* number1 = malloc(sizeof(int));
    *number1 = 2;
    int* number2 = malloc(sizeof(int));
    *number2 = 3;
    int* number3 = malloc(sizeof(int));
    *number3 = 4;
    int* number4 = malloc(sizeof(int));
    *number4 = 32;
    list_add(numbers, number1);
    list_add(numbers, number2);
    list_add(numbers, number3);
    list_add(numbers, number4);
    set_bf_ip(1,numbers);

    get_bf_ip(1);*/

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
        //instruction->op_code = F_CLOSE;
        //instruction->op_code = F_TRUNCATE;
        instruction->op_code = F_READ;
        //instruction->op_code = F_WRITE;

        instruction->args = list_create();
        char* argument = strdup("prueba1");
        list_add(instruction->args, argument);
        /*char* argument2 = strdup("64");//TAMANIO TRUNCATE
        list_add(instruction->args, argument2);*/
        char* argument4 = strdup("20");//TAMANIO LEER/ESCRIBIR
        list_add(instruction->args, argument4);
        char* argument5 = strdup("12");//POSICION
        list_add(instruction->args, argument5);//POSICION
        printf("El código de operación es: %i\n", instruction->op_code);
        switch (instruction->op_code) {
            case F_READ:
                printf("RECIBIMOS UNA INSTRUCCIÓN DE LEER ARCHIVO\n");
                char* miCharPuntero_r = malloc(sizeof(char));
                *miCharPuntero_r = read_file(instruction);
                if (!socket_send(config_fs.connection_kernel, serialize_message("OK_OPEN_FILE", false))) {
                    printf("Error al enviar el paquete\n");
                    return -1;
                }
                break;
            case F_WRITE:
                printf("RECIBIMOS UNA INSTRUCCIÓN DE LEER ARCHIVO\n");
                char* miCharPuntero_w = malloc(sizeof(char));
                *miCharPuntero_w = read_file(instruction);
                if (!socket_send(config_fs.connection_kernel, serialize_message("OK_OPEN_FILE", false))) {
                    printf("Error al enviar el paquete\n");
                    return -1;
                }
                break;
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
            case F_TRUNCATE:
                printf("RECIBIMOS UNA INSTRUCCIÓN DE TRUNCAR UN ARCHIVO\n");
                truncate_file(instruction);
                for(int i = 0; i < 256; i += sizeof(uint32_t)) {
                    uint32_t* number = malloc(sizeof(uint32_t));
                    memcpy(number, config_fs.block_file + i, sizeof(uint32_t));  // remove '*'
                    printf("Valor en la posición %d: %u\n", i, *number);
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

char read_file(t_instruction* instruction){
    if (list_size(instruction->args) < 1) {
        log_error(config_fs.logger, "Instrucción sin argumentos, se esperaba al menos uno");
        abort();
    }
    int size_read;
    char* str_read;
    if(instruction->op_code==F_READ){
        char* size_read_char = list_get(instruction->args, 1); // obtenemos la cadena que representa el tamaño
        size_read = atoi(size_read_char); // convertimos esa cadena a un entero
        str_read = calloc(size_read + 1, sizeof(char)); 
        for(int i = 0; i < 240; i++) {
            char c = config_fs.block_file[i];
            printf("Valor en la posición %d: %c\n", i, c);
        }
    }else{
        str_read = "SonyPlaystation5";
        size_read = (int) strlen(str_read);
    }
    char* file_name = list_get(instruction->args, 0);
    char* position_read_char = list_get(instruction->args, 2);

    // Convirtiéndolos a int
    int position_read = atoi(position_read_char);

    char* directorio = getcwd(NULL, 0);
	char* full_file_path = string_from_format("%s/cfg/%s%s.dat", directorio,config_fs.PATH_FCB, file_name);
    t_config* fcb_data = config_create(full_file_path);

    int PUNTERO_INDIRECTO = config_get_int_value(fcb_data, "PUNTERO_INDIRECTO");
    int* PUNTERO_DIRECTO = malloc(sizeof(int));
    *PUNTERO_DIRECTO = config_get_int_value(fcb_data, "PUNTERO_DIRECTO");
    t_list* pi_list = get_bf_ip(PUNTERO_INDIRECTO);
    list_add_in_index(pi_list, 0, PUNTERO_DIRECTO);

    int position_initial_block = (position_read / config_fs.block_size);
    int blocks_need = (size_read + config_fs.block_size - 1) / config_fs.block_size;
    int positions_to_read=position_read;
    int positions_readed=0;
    for (int i = position_initial_block; i <= position_initial_block + blocks_need ; i++) {
        int block_number = *(int*) list_get(pi_list, i);  // obtener el número del bloque
        int start_position_in_block = block_number * config_fs.block_size;  // posición inicial en el bloque //16
        int end_position_in_block = start_position_in_block + config_fs.block_size;  // posición final en el bloque //32
        int start_position_to_read=(positions_to_read-(i*config_fs.block_size))+start_position_in_block;
        for(int j=0; j<(end_position_in_block-start_position_to_read);j++){
            if(instruction->op_code==F_READ){
                char data = config_fs.block_file[start_position_to_read+j];
                if (data == '\0') {
                    str_read[positions_readed] = ' ';  
                } else {
                    str_read[positions_readed] = data;
                }
            }else{
                config_fs.block_file[start_position_to_read+j]=str_read[positions_readed];
            }
            positions_to_read++;
            positions_readed++;
            if(positions_readed == size_read) {
                break;  
            }
        }
        if(positions_readed == size_read) {
            break;  
        }
    }
    if(instruction->op_code==F_READ){
        str_read[positions_readed] = '\0';
        log_info(config_fs.logger, "El valor de la cadena es: %s", str_read);
    }else{
        for(int i = 0; i < 240; i++) {
            char c = config_fs.block_file[i];
            printf("Valor en la posición %d: %c\n", i, c);
        }
    }
    return *str_read;
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
        resize_block(fcb_data,&file_size);
        //set_bit_position(fcb_data,&file_size);
        //free(PUNTERO_DIRECTO); 
    }
    free(directorio); // liberar la memoria del getcwd
    free(full_file_path); 
    config_destroy(fcb_data); // usar config_destroy en lugar de free
    
}

void resize_block(t_config* fcb_data,  int* file_size){
    int PUNTERO_INDIRECTO = config_get_int_value(fcb_data, "PUNTERO_INDIRECTO");
    t_list* pi_list = get_bf_ip(PUNTERO_INDIRECTO);
    int list_length = list_size(pi_list);
    int list_size_value = list_size(pi_list);
    int diferencia = 0;
    int pi_position; 
    int count_pi_need = ((*file_size + config_fs.block_size - 1) / config_fs.block_size-1);
    if ((count_pi_need) > list_length) {
        diferencia = count_pi_need - list_length;
        for(int i = 0;i<diferencia;i++){
            pi_position = next_bit_position();
            int* pi_pos_ptr = malloc(sizeof(int));
            *pi_pos_ptr = pi_position;
            list_add(pi_list, pi_pos_ptr);
        }
        set_bf_ip(PUNTERO_INDIRECTO,pi_list);
    } else if ((count_pi_need) < list_length) {
        diferencia = list_length-count_pi_need ;
        for(int i = 0; i < diferencia; i++) {
            int index = (list_size_value - i - 1);
            int ip_eliminar = *((int*) list_get(pi_list, index));
            bitarray_clean_bit(config_fs.bitmap, ip_eliminar);
            for(int i = (ip_eliminar)*config_fs.block_size; i < ((ip_eliminar)*config_fs.block_size) + config_fs.block_size; i++) {
                config_fs.block_file[i] = '\0';
            }
            free(list_get(pi_list, index));
            list_remove(pi_list, index);
        }
        for(int i = PUNTERO_INDIRECTO*config_fs.block_size; i < (PUNTERO_INDIRECTO*config_fs.block_size) + config_fs.block_size; i++) {
            config_fs.block_file[i] = '\0';
        }
        set_bf_ip(PUNTERO_INDIRECTO,pi_list);
    }
}

void set_bf_ip(int PUNTERO_INDIRECTO, t_list* pi_list){
    int list_length = list_size(pi_list);
    for(int i = 0; i < list_length; i++) {
        int* pi_ptr = list_get(pi_list, i);
        uint32_t converted_pi = (uint32_t) *pi_ptr; // Aquí se desreferencia el puntero y se convierte el valor a uint32_t.
        memcpy(config_fs.block_file + (PUNTERO_INDIRECTO*config_fs.block_size) + i * sizeof(uint32_t), &converted_pi, sizeof(uint32_t));
    }
}

t_list* get_bf_ip(int PUNTERO_INDIRECTO){
    t_list* pi_list = list_create();
    for(int i = (PUNTERO_INDIRECTO*config_fs.block_size); i < ((PUNTERO_INDIRECTO*config_fs.block_size)+config_fs.block_size); i+= 4) {
        uint32_t* number = malloc(sizeof(uint32_t));
        memcpy(number, config_fs.block_file + i, sizeof(uint32_t));
        //printf("Valor en la posición %d: %u\n", i, *number);
        if(*number!=0){
            list_add(pi_list, number);
        }
    }
    return pi_list;
}
void clear_bit_position(t_config* fcb_data){
    int PUNTERO_DIRECTO = config_get_int_value(fcb_data, "PUNTERO_DIRECTO");
    for(int i = PUNTERO_DIRECTO*config_fs.block_size; i < (PUNTERO_DIRECTO*config_fs.block_size) + config_fs.block_size; i++) {
        config_fs.block_file[i] = 0;
    }
    int PUNTERO_INDIRECTO = config_get_int_value(fcb_data, "PUNTERO_INDIRECTO");
    t_list* pi_list = get_bf_ip(PUNTERO_INDIRECTO);
    for (int i = 0; i < list_size(pi_list); i++) {
        int pi_pos = *((int*) list_get(pi_list, i));
        bitarray_clean_bit(config_fs.bitmap, pi_pos);
        for(int j = pi_pos*config_fs.block_size; j < (pi_pos*config_fs.block_size) + config_fs.block_size; j++) {
            config_fs.block_file[j] = 0;
        }
    }
    for(int i = PUNTERO_INDIRECTO*config_fs.block_size; i < (PUNTERO_INDIRECTO*config_fs.block_size) + config_fs.block_size; i++) {
        config_fs.block_file[i] = 0;
    }
    list_destroy(pi_list);
    bitarray_clean_bit(config_fs.bitmap, PUNTERO_INDIRECTO);// liberamos el array de strings
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
    int pd_position=next_bit_position();
    char* pd_position_string = malloc(12 * sizeof(char));
    int pi_position=next_bit_position();
    char* pi_position_string = malloc(12 * sizeof(char));
    sprintf(pd_position_string, "%d", pd_position);
    sprintf(pi_position_string, "%d", pi_position);
    config_set_value(fcb_data, "PUNTERO_DIRECTO", pd_position_string); 
    config_set_value(fcb_data, "NOMBRE_ARCHIVO", file_name);
    config_set_value(fcb_data, "TAMANIO_ARCHIVO", ""); 
    config_set_value(fcb_data, "PUNTERO_DIRECTO", pd_position_string); 
    config_set_value(fcb_data, "PUNTERO_INDIRECTO", pi_position_string); 
    log_warning(config_fs.logger, "Creo FCB");

    free(pd_position_string);
    free(pi_position_string);
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
