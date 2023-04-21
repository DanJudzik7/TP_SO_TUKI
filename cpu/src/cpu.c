#include "cpu.h"
int main(int argc, char ** argv){

    printf("Iniciando la CPU\n");

    t_log* logger = iniciar_logger("cpu");
    t_config* config = iniciar_config("cpu");

    int cliente_fd = receive_modules(logger,config);

    //TODO: En el primer recv llega basura no se porque
    socket_recv_message(cliente_fd);
    socket_recv_message(cliente_fd);
}