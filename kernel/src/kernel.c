#include "kernel.h"


int main(int argc, char ** argv){

    printf("Iniciando el kernel");

    t_log* logger = iniciar_logger("kernel"); 

    t_config* config = iniciar_config("kernel");

    //Obtenemos el puerto con el que escucharemos conexiones
	char* puerto = config_get_string_value(config, "PUERTO_ESCUCHA");
    
    log_info(logger,"El valor del puerto es %s \n",puerto);    

    //Inicializo el socket en el puerto cargado por la config
    int server_fd = socket_initialize(puerto);
    log_info(logger,"SOCKET INICIALIZADO");    
    //Pongo el socket en modo de aceptar las escuchas
	int cliente_fd = socket_accept(server_fd);

	int recive_instrucction = instruction_handler_console(cliente_fd);
    if( recive_instrucction == (-1) ) printf("Hubo un error !!! "); //esto deberia pasar al logger
    
}



 
/* #include "kernel.h"

int main(int argc, char ** argv){

    printf("Iniciando el kernel");

    t_log* logger = iniciar_logger("kernel"); 

    t_config* config = iniciar_config("kernel");

    //Obtenemos el puerto con el que escucharemos conexiones
	char* puerto = config_get_string_value(config, "PUERTO_ESCUCHA");
    
    log_info(logger,"El valor del puerto es %s \n",puerto);    

    //Inicializo el socket en el puerto cargado por la config
    int server_fd = socket_initialize(puerto);
    log_info(logger,"SOCKET INICIALIZADO");    
    //Pongo el socket en modo de aceptar las escuchas
	int cliente_fd = socket_accept(server_fd);

    t_list* list_instructions;

    t_paquete* paquete = malloc(sizeof(t_paquete));
    paquete->buffer = malloc(sizeof(t_buffer));
	while (1) {

        // Primero recibimos el codigo de operacion
        int cod_op = socket_recv_operation(cliente_fd);
        if(cod_op==0){
            log_info(logger,"El codigo de operacion es: %i", cod_op);
            recv(cliente_fd, &(paquete->buffer->size), sizeof(uint32_t), MSG_WAITALL > 0);
            printf("prueba");
            paquete->buffer->stream = malloc(paquete->buffer->size);
            
            printf("prueba1");
            recv(cliente_fd, paquete->buffer->stream, paquete->buffer->size, MSG_WAITALL > 0);
            t_instruccion_package_0* inst = deserializar_instruction_0(paquete->buffer);
            
            printf("instruccion %s",inst->instruccion);


        } else if (cod_op==-1){
            log_info(logger,"Error al recibir codigo de operacion \n");
            return (-1);
        }

		
    }
}

t_instruccion_package_0* deserializar_instruction_0(t_buffer* buffer) {
    t_instruccion_package_0* inst = malloc(sizeof(inst));

    void* stream = buffer->stream;
    // Deserializamos los campos que tenemos en el buffer
    memcpy(&(inst->instruccion), stream, sizeof(uint32_t));
    stream += sizeof(uint32_t);
    //inst->instruccion = malloc(sizeof(inst->instruccion));

    return inst;
}*/ 