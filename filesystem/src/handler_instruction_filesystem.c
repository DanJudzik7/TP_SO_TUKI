#include "handler_instruction_filesystem.h"

extern config_filesystem config_fs;

int handle_kernel(int* socket_kernel) {
	while (1) {
		t_package* package = socket_receive(*socket_kernel);
		if (package == NULL) {
			log_error(config_fs.logger, "El kernel del socket %d se desconectó", *socket_kernel);
			break;
		}

		t_instruction* instruction = deserialize_instruction(package);
		log_warning(config_fs.logger, "El código de operación es: %i", instruction->op_code);
		bool processed = process_instruction(instruction);
		if (!socket_send(*socket_kernel, package_new(processed ? MESSAGE_OK : MESSAGE_FLAW))) {
			log_warning(config_fs.logger, "Error al enviar el paquete");
			return -1;
		}
		if (!processed) abort();
		free(instruction);
		free(package);
	}
	return 0;
}

bool process_instruction(t_instruction* instruction) {
	switch (instruction->op_code) {
		case F_READ: { // NAME(0) POS(1) SIZE(2) PID(3) S_ID(4) OFFSET(5) ORIGEN(6)-> s_id, offset, size, pid, origen
			printf("RECIBIMOS UNA INSTRUCCIÓN DE LEER ARCHIVO\n");
			char* read_file = iterate_block_file(instruction);
			if (config_fs.socket_memory != -1) {
				t_instruction* mem_request = instruction_new(MEM_WRITE_ADDRESS);
				list_add(mem_request->args, list_get(instruction->args, 4));
				list_add(mem_request->args, list_get(instruction->args, 5));
				list_add(mem_request->args, read_file);
				list_add(mem_request->args, list_get(instruction->args, 3));
				list_add(mem_request->args, "FS");
				if (!socket_send(config_fs.socket_memory, serialize_instruction(mem_request))) {
					log_error(config_fs.logger, "Error al enviar instrucciones a memoria");
				}
				t_package* package = socket_receive(config_fs.socket_memory);
				if (package->type == SEG_FAULT) {
					log_error(config_fs.logger, "Segmentation Fault al leer el archivo");
					return false;
				}
				if (package->type != MESSAGE_OK) {
					log_error(config_fs.logger, "Error desconocido al leer el archivo");
					return false;
				}
				instruction_destroy(mem_request);
			}
			free(read_file);
			return true;
		}
		case F_WRITE: { // NAME(0) POS(1) SIZE(2) PID(3) S_ID(4) OFFSET(5) ORIGEN(6)-> s_id, offset, size, pid, origen
			printf("RECIBIMOS UNA INSTRUCCIÓN DE ESCRIBIR ARCHIVO\n");
			if (config_fs.socket_memory != -1) {
				t_instruction* mem_request = instruction_new(MEM_READ_ADDRESS);
				list_add(mem_request->args, list_get(instruction->args, 4));
				list_add(mem_request->args, list_get(instruction->args, 5));
				list_add(mem_request->args, list_get(instruction->args, 2));
				list_add(mem_request->args, list_get(instruction->args, 3));
				list_add(mem_request->args, "FS");
				if (!socket_send(config_fs.socket_memory, serialize_instruction(mem_request))) {
					log_error(config_fs.logger, "Error al enviar instrucciones a memoria");
				}
				t_package* package_receive_memory = socket_receive(config_fs.socket_memory);
				char* to_write = deserialize_message(package_receive_memory);
				list_add(instruction->args, to_write);
				instruction_destroy(mem_request);
			}
			char* result_write = iterate_block_file(instruction);
			free(result_write);
			return true;
		}
		case F_OPEN: {
			printf("RECIBIMOS UNA INSTRUCCIÓN DE ABRIR ARCHIVO\n");
			open_file(instruction);
			return true;
		}
		case F_CLOSE: {
			printf("RECIBIMOS UNA INSTRUCCIÓN DE CERRAR UN ARCHIVO\n");
			//close_file(instruction);
			return true;
		}
		case F_TRUNCATE: {
			printf("RECIBIMOS UNA INSTRUCCIÓN DE TRUNCAR UN ARCHIVO\n");
			truncate_file(instruction);
			/*for (int i = 0; i < config_fs.block_count; i++) {
				bool car = bitarray_test_bit(config_fs.bitmap, i);
				printf("BIT:%i %s\n", i, car ? "Verdadero" : "Falso");
			}
			for (int i = 0; i < 256; i += sizeof(uint32_t)) {
				uint32_t* number = s_malloc(sizeof(uint32_t));
				memcpy(number, config_fs.block_file + i, sizeof(uint32_t));	 // remove '*'
				printf("Valor en la posición %d: %u\n", i, *number);
			}*/
			return true;
		}
		default: {
			printf("Error al recibir código de operación\n");
			return false;
		}
	}
}

char* iterate_block_file(t_instruction* instruction) {
	if (list_size(instruction->args) < 1) {
		log_error(config_fs.logger, "Instrucción sin argumentos, se esperaba al menos uno");
		abort();
	}

	char* file_name = list_get(instruction->args, 0);
	int position_read = atoi(list_get(instruction->args, 1));
	int size_read = atoi(list_get(instruction->args, 2));
	char* direccion_memoria = list_get(instruction->args, 6);
	char* str_read;
	if (instruction->op_code == F_READ) {
		log_info(config_fs.logger, "Leer Archivo: %s - Puntero: %i - Memoria: %s - Tamaño: %i", file_name, position_read, direccion_memoria, size_read) ;
		str_read = calloc(size_read + 1, sizeof(char));
		/*for (int i = 0; i < 240; i++) {
			printf("Valor en la posición %d: %c\n", i, config_fs.block_file[i]);
		}*/
	} else {
		log_info(config_fs.logger, "Escribir Archivo: %s - Puntero: %i - Memoria: %s - Tamaño: %i", file_name, position_read, direccion_memoria, size_read) ;
		str_read = list_get(instruction->args, 7);
		log_info(config_fs.logger, "El valor de la cadena a escribir es: %s", str_read);
	}
	char* directorio = getcwd(NULL, 0);
	char* full_file_path = string_from_format("%s/cfg/%s%s.dat", directorio, config_fs.PATH_FCB, file_name);
	t_config* fcb_data = config_create(full_file_path);
	int PUNTERO_INDIRECTO = config_get_int_value(fcb_data, "PUNTERO_INDIRECTO");
	char* puntero_indirecto_char = config_get_string_value(fcb_data, "PUNTERO_INDIRECTO");
	if (strcmp(puntero_indirecto_char, "") != 0) {
		log_info(config_fs.logger, "Acceso Bloque - Archivo: %s - Bloque Archivo: %i - Bloque File System: %i ", file_name, PUNTERO_INDIRECTO, PUNTERO_INDIRECTO);
	}
	int* PUNTERO_DIRECTO = s_malloc(sizeof(int));
	*PUNTERO_DIRECTO = config_get_int_value(fcb_data, "PUNTERO_DIRECTO");
	t_list* pi_list = get_bf_ip(PUNTERO_INDIRECTO);
	list_add_in_index(pi_list, 0, PUNTERO_DIRECTO);
	int position_initial_block = (position_read / config_fs.block_size);
	int blocks_need = (size_read + config_fs.block_size - 1) / config_fs.block_size;
	int positions_to_read = position_read;
	int read_positions = 0;
	for (int i = position_initial_block; i <= position_initial_block + blocks_need; i++) {
		int block_number = *(int*)list_get(pi_list, i);
		log_info(config_fs.logger, "Acceso Bloque - Archivo: %s - Bloque Archivo: %i - Bloque File System: %i ", file_name, block_number, block_number);
		int start_position_in_block = block_number * config_fs.block_size;
		int end_position_in_block = start_position_in_block + config_fs.block_size;
		int start_position_to_read = (positions_to_read - (i * config_fs.block_size)) + start_position_in_block;
		for (int j = 0; j < (end_position_in_block - start_position_to_read); j++) {
			if (instruction->op_code == F_READ) {
				char data = config_fs.block_file[start_position_to_read + j];
				str_read[read_positions] = data != '\0' ? data : ' ';
			} else {
				config_fs.block_file[start_position_to_read + j] = str_read[read_positions];
			}
			positions_to_read++;
			read_positions++;
			if (read_positions == size_read) break;
		}
		if (read_positions == size_read) break;
	}
	if (instruction->op_code == F_READ) {
		str_read[read_positions] = '\0';
		log_info(config_fs.logger, "El valor de la cadena leida es: %s", str_read);
	} else {
		log_info(config_fs.logger, "Escribio correctamente el Archivo: %s - Puntero: %i - Memoria: %s - Tamaño: %i", file_name, position_read, direccion_memoria, size_read) ;
		for (int i = 0; i < 240; i++) {
			char c = config_fs.block_file[i];
			printf("Valor en la posición %d: %c\n", i, c);
		}
	}
	free(file_name);
	free(directorio);
	free(full_file_path);
	free(direccion_memoria);
	config_destroy(fcb_data);
	list_destroy_and_destroy_elements(pi_list, free);
	return str_read;
}
void truncate_file(t_instruction* instruction) {
	if (list_size(instruction->args) < 1) {
		log_error(config_fs.logger, "Instrucción sin argumentos, se esperaba al menos uno");
		abort();
	}
	char* file_name = list_get(instruction->args, 0);
	char* char_file_size = list_get(instruction->args, 1);
	int file_size = atoi(char_file_size);
	char* directorio = getcwd(NULL, 0);
	char* full_file_path = string_from_format("%s/cfg/%s%s.dat", directorio, config_fs.PATH_FCB, file_name);
	t_config* fcb_data = config_create(full_file_path);
	log_info(config_fs.logger, "Truncar Archivo: %s - Tamaño: %i",file_name, file_size);
	if (fcb_data == NULL) {
		open_file(instruction);
		truncate_file(instruction);
	} else {
		resize_block(fcb_data, &file_size);
		// set_bit_position(fcb_data,&file_size);
		// free(PUNTERO_DIRECTO);
	}
	log_info(config_fs.logger, "Archivo truncado: %s",file_name);
	free(directorio);  // liberar la memoria del getcwd
	free(full_file_path);
	config_destroy(fcb_data);  // usar config_destroy en lugar de free
}

void resize_block(t_config* fcb_data, int* file_size) {
	int TAMANIO_ARCHIVO = config_get_int_value(fcb_data, "TAMANIO_ARCHIVO");
	char* size_file_char = string_itoa(*file_size);
	config_set_value(fcb_data, "TAMANIO_ARCHIVO", size_file_char);
	if (*file_size != TAMANIO_ARCHIVO) {
		char* puntero_directo_char = config_get_string_value(fcb_data, "PUNTERO_DIRECTO");
		char* puntero_indirecto_char = config_get_string_value(fcb_data, "PUNTERO_INDIRECTO");
		int PUNTERO_DIRECTO = config_get_int_value(fcb_data, "PUNTERO_DIRECTO");
		int PUNTERO_INDIRECTO = config_get_int_value(fcb_data, "PUNTERO_INDIRECTO");

		t_list* pi_list = get_bf_ip(PUNTERO_INDIRECTO);
		int list_length = list_size(pi_list);
		if (*file_size == 0) {
			if (strcmp(puntero_directo_char, "") != 0) {
				for (int i = PUNTERO_DIRECTO * config_fs.block_size; i < (PUNTERO_DIRECTO * config_fs.block_size) + config_fs.block_size; i++) {
					config_fs.block_file[i] = '\0';
				}

				log_info(config_fs.logger, "Acceso a Bitmap - Bloque: %i - Estado: 1 - OCUPADO",PUNTERO_DIRECTO);
				bitarray_clean_bit(config_fs.bitmap, PUNTERO_DIRECTO);
				log_info(config_fs.logger, "Acceso a Bitmap - Bloque: %i - Estado: 0 - LIBRE",PUNTERO_DIRECTO);
				config_set_value(fcb_data, "PUNTERO_DIRECTO", "");
			}
			if (strcmp(puntero_indirecto_char, "") != 0) {
				for (int i = 0; i < list_length; i++) {
					int pi_pos = *((int*)list_get(pi_list, i));
					log_info(config_fs.logger, "Acceso a Bitmap - Bloque: %i - Estado: 1 - OCUPADO", pi_pos);
					bitarray_clean_bit(config_fs.bitmap, pi_pos);
					log_info(config_fs.logger, "Acceso a Bitmap - Bloque: %i - Estado: 0 - LIBRE", pi_pos);
					for (int j = pi_pos * config_fs.block_size; j < (pi_pos * config_fs.block_size) + config_fs.block_size; j++) {
						config_fs.block_file[j] = '\0';
					}
				}
				for (int i = PUNTERO_INDIRECTO * config_fs.block_size; i < (PUNTERO_INDIRECTO * config_fs.block_size) + config_fs.block_size; i++) {
					config_fs.block_file[i] = '\0';
				}
				list_destroy(pi_list);
				log_info(config_fs.logger, "Acceso a Bitmap - Bloque: %i - Estado: 1 - OCUPADO", PUNTERO_INDIRECTO);
				bitarray_clean_bit(config_fs.bitmap, PUNTERO_INDIRECTO);
				log_info(config_fs.logger, "Acceso a Bitmap - Bloque: %i - Estado: 0 - LIBRE", PUNTERO_INDIRECTO);
				config_set_value(fcb_data, "PUNTERO_INDIRECTO", "");
			}
		} else if ((*file_size) <= config_fs.block_size) {
			if (strcmp(puntero_directo_char, "") == 0) {
				char* pd_position_string = string_itoa(next_bit_position());
				config_set_value(fcb_data, "PUNTERO_DIRECTO", pd_position_string);
			}
			if (strcmp(puntero_indirecto_char, "") != 0) {
				for (int i = 0; i < list_length; i++) {
					int pi_pos = *((int*)list_get(pi_list, i));
					log_info(config_fs.logger, "Acceso a Bitmap - Bloque: %i - Estado: 1 - OCUPADO", pi_pos);
					bitarray_clean_bit(config_fs.bitmap, pi_pos);
					log_info(config_fs.logger, "Acceso a Bitmap - Bloque: %i - Estado: 0 - LIBRE", pi_pos);
					for (int j = pi_pos * config_fs.block_size; j < (pi_pos * config_fs.block_size) + config_fs.block_size; j++) {
						config_fs.block_file[j] = '\0';
					}
				}
				for (int i = PUNTERO_INDIRECTO * config_fs.block_size; i < (PUNTERO_INDIRECTO * config_fs.block_size) + config_fs.block_size; i++) {
					config_fs.block_file[i] = '\0';
				}
				list_destroy(pi_list);
				log_info(config_fs.logger, "Acceso a Bitmap - Bloque: %i - Estado: 1 - OCUPADO", PUNTERO_INDIRECTO);
				bitarray_clean_bit(config_fs.bitmap, PUNTERO_INDIRECTO);
				log_info(config_fs.logger, "Acceso a Bitmap - Bloque: %i - Estado: 0 - LIBRE", PUNTERO_INDIRECTO);
				config_set_value(fcb_data, "PUNTERO_INDIRECTO", "");
			}
		} else {
			if (strcmp(puntero_directo_char, "") == 0) {
				char* pd_position_string = string_itoa(next_bit_position());
				config_set_value(fcb_data, "PUNTERO_DIRECTO", pd_position_string);
				PUNTERO_DIRECTO = atoi(pd_position_string);
			}
			if (strcmp(puntero_indirecto_char, "") == 0) {
				char* pi_position_string = string_itoa(next_bit_position());
				config_set_value(fcb_data, "PUNTERO_INDIRECTO", pi_position_string);
				PUNTERO_INDIRECTO = atoi(pi_position_string);
			}
			int diferencia = 0;
			int pi_position;
			int count_pi_need = ((*file_size + config_fs.block_size - 1) / config_fs.block_size - 1);
			if ((count_pi_need) > list_length) {
				diferencia = count_pi_need - list_length;
				for (int i = 0; i < diferencia; i++) {
					pi_position = next_bit_position();
					int* pi_pos_ptr = s_malloc(sizeof(int));
					*pi_pos_ptr = pi_position;
					list_add(pi_list, pi_pos_ptr);
				}
				set_bf_ip(PUNTERO_INDIRECTO, pi_list);
			} else if ((count_pi_need) < list_length) {
				diferencia = list_length - count_pi_need;
				for (int i = 0; i < diferencia; i++) {
					int index = (list_length - i - 1);
					int ip_eliminar = *((int*)list_get(pi_list, index));
					log_info(config_fs.logger, "Acceso a Bitmap - Bloque: %i - Estado: 1 - OCUPADO", ip_eliminar);
					bitarray_clean_bit(config_fs.bitmap, ip_eliminar);
					log_info(config_fs.logger, "Acceso a Bitmap - Bloque: %i - Estado: 0 - LIBRE", ip_eliminar);
					for (int i = (ip_eliminar)*config_fs.block_size; i < ((ip_eliminar)*config_fs.block_size) + config_fs.block_size; i++) {
						config_fs.block_file[i] = '\0';
					}
					free(list_get(pi_list, index));
					list_remove(pi_list, index);
				}
				for (int i = PUNTERO_INDIRECTO * config_fs.block_size; i < (PUNTERO_INDIRECTO * config_fs.block_size) + config_fs.block_size; i++) {
					config_fs.block_file[i] = '\0';
				}
				set_bf_ip(PUNTERO_INDIRECTO, pi_list);
			}
		}
	}
	config_save(fcb_data);
}

void set_bf_ip(int PUNTERO_INDIRECTO, t_list* pi_list) {
	int list_length = list_size(pi_list);
	for (int i = 0; i < list_length; i++) {
		int* pi_ptr = list_get(pi_list, i);
		uint32_t converted_pi = (uint32_t)*pi_ptr;	// Aquí se desreferencia el puntero y se convierte el valor a uint32_t.
		memcpy(config_fs.block_file + (PUNTERO_INDIRECTO * config_fs.block_size) + i * sizeof(uint32_t), &converted_pi, sizeof(uint32_t));
	}
}

t_list* get_bf_ip(int PUNTERO_INDIRECTO) {
	t_list* pi_list = list_create();
	for (int i = (PUNTERO_INDIRECTO * config_fs.block_size); i < ((PUNTERO_INDIRECTO * config_fs.block_size) + config_fs.block_size); i += 4) {
		uint32_t* number = s_malloc(sizeof(uint32_t));
		memcpy(number, config_fs.block_file + i, sizeof(uint32_t));
		// printf("Valor en la posición %d: %u\n", i, *number);
		if (*number != 0) {
			list_add(pi_list, number);
		}
	}
	return pi_list;
}
void clear_bit_position(t_config* fcb_data) {
	int PUNTERO_DIRECTO = config_get_int_value(fcb_data, "PUNTERO_DIRECTO");
	for (int i = PUNTERO_DIRECTO * config_fs.block_size; i < (PUNTERO_DIRECTO * config_fs.block_size) + config_fs.block_size; i++) {
		config_fs.block_file[i] = '\0';
	}
	int PUNTERO_INDIRECTO = config_get_int_value(fcb_data, "PUNTERO_INDIRECTO");
	t_list* pi_list = get_bf_ip(PUNTERO_INDIRECTO);
	for (int i = 0; i < list_size(pi_list); i++) {
		int pi_pos = *((int*)list_get(pi_list, i));
		log_info(config_fs.logger, "Acceso a Bitmap - Bloque: %i - Estado: 1 - OCUPADO", pi_pos);
		bitarray_clean_bit(config_fs.bitmap, pi_pos);
		log_info(config_fs.logger, "Acceso a Bitmap - Bloque: %i - Estado: 0 - LIBRE", pi_pos);
		for (int j = pi_pos * config_fs.block_size; j < (pi_pos * config_fs.block_size) + config_fs.block_size; j++) {
			config_fs.block_file[j] = '\0';
		}
	}
	for (int i = PUNTERO_INDIRECTO * config_fs.block_size; i < (PUNTERO_INDIRECTO * config_fs.block_size) + config_fs.block_size; i++) {
		config_fs.block_file[i] = '\0';
	}
	list_destroy(pi_list);
	log_info(config_fs.logger, "Acceso a Bitmap - Bloque: %i - Estado: 1 - OCUPADO", PUNTERO_INDIRECTO);
	bitarray_clean_bit(config_fs.bitmap, PUNTERO_INDIRECTO);
	log_info(config_fs.logger, "Acceso a Bitmap - Bloque: %i - Estado: 0 - LIBRE", PUNTERO_INDIRECTO);
	log_info(config_fs.logger, "Acceso a Bitmap - Bloque: %i - Estado: 1 - OCUPADO", PUNTERO_DIRECTO);  // liberamos el array de strings
	bitarray_clean_bit(config_fs.bitmap, PUNTERO_DIRECTO);
	log_info(config_fs.logger, "Acceso a Bitmap - Bloque: %i - Estado: 0 - LIBRE", PUNTERO_DIRECTO);
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
	char* full_file_path = string_from_format("%s/cfg/%s%s.dat", directorio, config_fs.PATH_FCB, file_name);

	log_info(config_fs.logger, "Abrir Archivo: %s",file_name);
	FILE* fcb = fopen(full_file_path, "r");
	if (fcb == NULL) {
		fopen(full_file_path, "w");
		log_warning(config_fs.logger, "No existía el archivo");
		create_file(full_file_path, file_name);
	} else {
		log_warning(config_fs.logger, "Existía el archivo");
		fclose(fcb);
	}
	log_info(config_fs.logger, "Archivo abierto: %s",file_name);
	free(full_file_path);
	free(directorio);  // liberamos directorio
}
void create_file(char* full_file_path, char* file_name) {
	log_info(config_fs.logger, "Crear Archivo: %s",file_name);
	t_config* fcb_data = config_create(full_file_path);
	if (fcb_data == NULL) {
		log_warning(config_fs.logger, "Error creando FCB");	 // liberamos full_file_path antes de abortar
		abort();
	}
	log_info(config_fs.logger, "Archivo creado: %s",file_name);
	config_set_value(fcb_data, "NOMBRE_ARCHIVO", file_name);
	config_set_value(fcb_data, "TAMANIO_ARCHIVO", "");
	config_set_value(fcb_data, "PUNTERO_DIRECTO", "");
	config_set_value(fcb_data, "PUNTERO_INDIRECTO", "");

	config_save(fcb_data);
	config_destroy(fcb_data);  // liberamos full_file_path
}

void close_file(t_instruction* instruction) {
	char* file_name = list_get(instruction->args, 0);
	char* directorio = getcwd(NULL, 0);
	char* full_file_path = string_from_format("%s/cfg/%s%s.dat", directorio, config_fs.PATH_FCB, file_name);
	t_config* fcb_data = config_create(full_file_path);
	clear_bit_position(fcb_data);

	/*int remove_status = remove(full_file_path);
	if (remove_status == 0) {
		log_info(config_fs.logger, "El archivo %s ha sido cerrado exitosamente", full_file_path);
	} else {
		log_error(config_fs.logger, "Error al intentar cerrar el archivo %s", full_file_path);
		config_destroy(fcb_data);  // liberamos fcb_data antes de abortar
		free(full_file_path);	   // liberamos full_file_path antes de abortar
		abort();
	}*/
	config_destroy(fcb_data);  // liberamos fcb_data
	free(full_file_path);	   // liberamos full_file_path
	free(directorio);		   // liberamos directorio
}

int next_bit_position() {
	for (int i = 0; i < config_fs.block_count; i++) {
		// bool car = bitarray_test_bit(config_fs.bitmap, i);
		// printf("BIT:%i %s\n", i, car ? "Verdadero" : "Falso");
		if (!bitarray_test_bit(config_fs.bitmap, i)) {
			log_info(config_fs.logger, "Acceso a Bitmap - Bloque: %i - Estado: 0 - LIBRE", i);
			bitarray_set_bit(config_fs.bitmap, i);
			log_info(config_fs.logger, "Acceso a Bitmap - Bloque: %i - Estado: 1 - OCUPADO", i);
			return i;
		}else{
			log_info(config_fs.logger, "Acceso a Bitmap - Bloque: %i - Estado: 1 - OCUPADO", i);
		}
	}
	return -1;
}
