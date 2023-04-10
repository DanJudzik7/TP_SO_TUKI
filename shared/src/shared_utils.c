#include "shared_utils.h"

char* mi_funcion_compartida(){
    return "Hice uso de la shared!";
}

t_config* iniciar_config(char* process_name){

    t_config *config = NULL;
    char* path_config;

    path_config=obtener_cfg_type(process_name,"config");

    printf("Directorio actual: %s\n", path_config);
    printf("Ruta del archivo de configuracion: %s\n", path_config);

    config = config_create(path_config);

    if (config == NULL) {
        printf("Error al cargar la configuracion\n");
        free(path_config);
        exit(1);
    }

    free(path_config);

    return config;
}

t_log* iniciar_logger(char* process_name){
    t_log *logger;
    char *path_log;
    

   path_log=obtener_cfg_type(process_name,"log");


	if((logger=log_create(path_log, process_name,true, LOG_LEVEL_INFO)) == NULL){
		printf("Error crear logger");
		exit(1);
	}else{
		return logger;
	}
}

char* obtener_cfg_type(char* process_name, char *file_type){

    char *directorio = getcwd(NULL, 0);

    if (directorio == NULL) {
        printf("Error al obtener el directorio actual\n");
        exit(1);
    } 

    char *ruta_config = malloc(strlen(directorio) + strlen("/cfg/.") + strlen(process_name)+ strlen(".") + strlen(file_type)+ 1);
    sprintf(ruta_config, "%s/cfg/%s.%s", directorio,process_name, file_type );
    
    if (ruta_config == NULL) {
        printf("Error al reservar memoria\n");
        free(directorio);
        exit(1);
    }else{
        return ruta_config;
    }
}
