#include "memoria.h"

int main(int argc, char ** argv){    
    t_log* logger = start_logger("memoria");
    t_config* config = start_config("memoria");
    log_warning(logger, "Iniciando la memoria");

    int cliente_fd = receive_modules(logger,config);

    //TODO: En el primer recv llega basura no se porque
    socket_receive_message(cliente_fd);
    socket_receive_message(cliente_fd);  
}