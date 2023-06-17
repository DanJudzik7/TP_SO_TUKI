#include "handler_instruction_filesystem.h"

int instruction_handler_filesystem(configuration_filesystem *config_fs) {
	// Todo esto no está adaptado al nuevo packaging
	while (1) {
		t_package* package = socket_receive(config_fs->connection_kernel);
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
                open_file(config_fs,instruction);
				if (!socket_send(config_fs->connection_kernel, serialize_message("OK_OPEN_FILE", false))) {
					printf("Error al enviar el paquete\n");
					return -1;
				}
				break;
			case F_CLOSE:
				printf("RECIBIMOS UNA INSTRUCCIÓN DE CERRAR UN ARCHIVO\n");
                close_file(config_fs,instruction);
				if (!socket_send(config_fs->connection_kernel, serialize_message("OK_CLOSE_FILE", false))) {
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
void open_file(configuration_filesystem* config_fs, t_instruction* instruction) {
    if (list_size(instruction->args) < 1) {
        log_error(config_fs->logger, "Instrucción sin argumentos, se esperaba al menos uno");
        abort();
    }
    char* file_name = list_get(instruction->args, 0);
    char* full_file_path = malloc(strlen(config_fs->PATH_FCB) + strlen(file_name) + strlen(".dat") + 2);
    sprintf(full_file_path, "%s/%s.dat", config_fs->PATH_FCB, file_name);

    FILE* file = fopen(full_file_path, "a"); // Abre el archivo en modo de adición. Si el archivo no existe, lo crea.

    if (file == NULL) {
        log_error(config_fs->logger, "Error al abrir o crear el archivo %s", full_file_path);
        abort();
    }

    fclose(file); // No olvides cerrar el archivo cuando hayas terminado con él
    free(full_file_path); // No olvides liberar la memoria que reservaste
}
void close_file(configuration_filesystem* config_fs, t_instruction* instruction) {
    char* file_name = list_get(instruction->args, 0);
    char* full_file_path = malloc(strlen(config_fs->PATH_FCB) + strlen(file_name) + strlen(".dat") + 2);
    sprintf(full_file_path, "%s/%s.dat", config_fs->PATH_FCB, file_name);
    
    // Intentar eliminar el archivo
    int remove_status = remove(full_file_path);
    if (remove_status == 0) {
        log_info(config_fs->logger, "El archivo %s ha sido cerrado exitosamente", full_file_path);
    } else {
        // Si hay un error, registrar en el log. Esto podría ser porque el archivo no existía.
        log_error(config_fs->logger, "Error al intentar cerrar el archivo %s", full_file_path);
        abort();
    }
    
    free(full_file_path);
}