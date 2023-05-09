#include "memoria.h"

configuration_memory config_memory;
memory memory_shared;

int main(int argc, char ** argv){    
    config_memory.logger = start_logger("memoria");
    t_config* config = start_config("memoria");
    config_memory.algorithm = config_get_string_value(config, "ALGORITMO_PLANIFICACION");
    memory_shared.segment_zero = config_get_string_value(config, "TAM_SEGMENTO_0");
    memory_shared.size = config_get_string_value(config, "TAM_MEMORIA");
    log_warning(config_memory.logger, "Iniciando la memoria");

	char* port = config_get_string_value(config, "PUERTO_ESCUCHA");	
	int socket_memory = socket_initialize_server(port);	// Inicializo el socket en el puerto cargado por la config
	if (socket_memory == -1) {
		log_error(config_memory.logger, "No se pudo inicializar el socket de servidor");
		exit(EXIT_FAILURE);
	}
	log_warning(config_memory.logger, "Socket de servidor inicializado en puerto %s", port);

    int cliente_fd = receive_modules(config_memory.logger,config);

    //TODO: En el primer recv llega basura no se porque  
    instruction_handler_memoria(cliente_fd);
   
}