#include "handler_instruction_consola.h"

void list_instruccions(t_list *instruccions)
{
    char *directorio = getcwd(NULL, 0);
    char *file_directory = malloc(strlen(directorio) + strlen("/cfg/instrucciones") + 1);
    sprintf(file_directory, "%s/cfg/instrucciones", directorio);
    
    FILE *file_instruccions;
    file_instruccions = fopen(file_directory, "r");
    if (file_instruccions == NULL) {
        printf("Error al abrir el archivo\n");
        exit(1);
    }
   
    char buffer[512];
    while(fgets(buffer,sizeof(buffer),file_instruccions)){
        char *line = strdup(buffer); // crear una copia de la línea
        //printf("lei: %s",line);
        line[strcspn(line, "\n")] = '\0'; //Eliminamos el fin de linea
        list_add(instruccions,line); //agrega los elementos leidos a la lista de instrucciones
        
    }

    fclose(file_instruccions);
    free(file_directory);
}

// UNICAMENTE PARA VER QUE CONTIENE LA LISTA ITERADA, dps volar
void iterator(char* value){
	printf("%s\n", value);
}

//Procesamos la instruccion, y la adherimos y enviamos por paquete una por una
void process_instruccions(t_list* instruccions, int socket_server){ 

	//Leemos la linea de instrucciones una por una
    for (int i = 0; i < list_size(instruccions); i++) {
		//Devuelve la linea de instruccion 
   		 char*  instruccion = list_get(instruccions, i);
    	//Manejo la linea de instrucciones, la cual devuelve un paquete listo para enviar 
    	t_paquete* paquete = handler_instruction_package(instruccion);
		//envio el paquete al server
    	socket_send_package(paquete, socket_server);
    }
}

int string_array_length(char** array) {
    int count = 0;
    while (array[count] != NULL) {
        count++;
    }
    return count;
}

//Maneja la instrucciones de la linea, y nos devuelve un paquete
t_paquete* handler_instruction_package(char* lineinstruccion){
    
    //Devuelve una lista de arreglo string con las instrucciones separadas por un espacio
    char** parametros= string_split( lineinstruccion , " ");
    op_code cod_op = return_opcode(parametros[0]);
	//Creamos el paquete con el operation code correspondiente
    t_paquete* paquete = create_package(cod_op);
    
    for(int i = 1;i < string_array_length(parametros) ;i++){
         add_to_package(paquete, parametros[i], strlen(parametros[i]) + 1);
    }

    return paquete;
}
// TODO ESTA OPERACION TENDRIA QUE IR AL SHARED
op_code_reception opcode_recive(char* code) {
     if (strcmp(code, "RECIVE_OK") == 0) {
        return OK;
    } else if (strcmp(code, "ERROR") == 0) {
        return ERROR;
    } else {
        return (-1); // Otra opción de manejo de error, si lo deseas
    }
}