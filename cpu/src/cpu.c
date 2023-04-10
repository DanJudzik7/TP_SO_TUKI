#include "cpu.h"
int main(int argc, char ** argv){

    printf("Iniciando la CPU");

    if(argc > 1 && strcmp(argv[1],"-test")==0)
        return run_tests();
    else{  
        t_log* logger = log_create("./cfg/cpu.log", "cpu", true, LOG_LEVEL_INFO);
        log_info(logger, "Soy el cpu! %s", mi_funcion_compartida());
        log_destroy(logger);
    } 
    
}