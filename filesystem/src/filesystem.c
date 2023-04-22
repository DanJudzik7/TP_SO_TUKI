#include "filesystem.h"
int main(int argc, char ** argv){

    printf("Iniciando el filesystem");

    t_log* logger = iniciar_logger("filesystem");
    t_config* config = iniciar_config("filesystem");

    int cliente_fd = receive_modules(logger,config);

    //TODO: En el primer recv llega basura no se porque
    socket_recv_message(cliente_fd);
    socket_recv_message(cliente_fd); 

    int conexion_memoria = conect_modules(config,logger,"MEMORIA"); 
}