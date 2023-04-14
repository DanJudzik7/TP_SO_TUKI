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
    //Limpio las lineas por si tienen alguna separacion rara
	string_trim(lineinstruccion);
    //Devuelve una lista de arreglo string con las instrucciones separadas por un espacio
    char** parametros= string_split( lineinstruccion , " ");
    op_code cod_op = return_opcode(parametros[0]);
	//Creamos el paquete 
    t_paquete* paquete = create_package(cod_op);
    
    for(int i = 1;i < string_array_length(parametros) ;i++){
         add_to_package(paquete, parametros[i], strlen(parametros[i] + 1));
    }

    return paquete;
}
//Devuelve el op_code dependiendo el mensaje leido
op_code return_opcode(char* code) {

    if (strcmp(code, "MENSAJE") == 0) {
        return MENSAJE;
    } else if (strcmp(code, "F_READ") == 0) {
        return F_READ;
    } else if (strcmp(code, "F_WRITE") == 0) {
        return F_WRITE;
    } else if (strcmp(code, "SET") == 0) {
        return SET;
    } else if (strcmp(code, "MOV_IN") == 0) {
        return MOV_IN;
    } else if (strcmp(code, "MOV_OUT") == 0) {
        return MOV_OUT;
    } else if (strcmp(code, "F_TRUNCATE") == 0) {
        return F_TRUNCATE;
    } else if (strcmp(code, "F_SEEK") == 0) {
        return F_SEEK;
    } else if (strcmp(code, "CREATE_SEGMENT") == 0) {
        return CREATE_SEGMENT;
    } else if (strcmp(code, "I/O") == 0) {
        return I_O;
    } else if (strcmp(code, "WAIT") == 0) {
        return WAIT;
    } else if (strcmp(code, "SIGNAL") == 0) {
        return SIGNAL;
    } else if (strcmp(code, "F_OPEN") == 0) {
        return F_OPEN;
    } else if (strcmp(code, "F_CLOSE") == 0) {
        return F_CLOSE;
    } else if (strcmp(code, "DELETE_SEGMENT") == 0) {
        return DELETE_SEGMENT;
    } else if (strcmp(code, "EXIT") == 0) {
        return EXIT;
    } else if (strcmp(code, "YIELD") == 0) {
        return YIELD;
    } else {
        return (-1); // Otra opción de manejo de error, si lo deseas
    }
}