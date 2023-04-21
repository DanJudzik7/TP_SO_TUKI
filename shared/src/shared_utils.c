#include "shared_utils.h"

char* deserialize_payload(t_list* payload){
    return "IMPLEMENTAR";
}

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

void* serialize_package(t_paquete* paquete, int bytes)
{
	void * magic = malloc(bytes);
	int desplazamiento = 0;

	memcpy(magic + desplazamiento, &(paquete->codigo_operacion), sizeof(int));
	desplazamiento+= sizeof(int);
	memcpy(magic + desplazamiento, &(paquete->buffer->size), sizeof(int));
	desplazamiento+= sizeof(int);
	memcpy(magic + desplazamiento, paquete->buffer->stream, paquete->buffer->size);
	desplazamiento+= paquete->buffer->size;

	return magic;
}

void add_to_package(t_paquete* paquete, void* valor, int tamanio) 
{
	paquete->buffer->stream = realloc(paquete->buffer->stream, paquete->buffer->size + tamanio + sizeof(int));

	memcpy(paquete->buffer->stream + paquete->buffer->size, &tamanio, sizeof(int));
	memcpy(paquete->buffer->stream + paquete->buffer->size + sizeof(int), valor, tamanio);

	paquete->buffer->size += tamanio + sizeof(int);
}

void create_buffer(t_paquete* paquete)
{
	paquete->buffer = malloc(sizeof(t_buffer));
	paquete->buffer->size = 0;
	paquete->buffer->stream = NULL;
}

t_paquete* create_package(int cod_op){ 
	t_paquete* paquete = malloc(sizeof(t_paquete));
	paquete->codigo_operacion = cod_op;
	create_buffer(paquete);
	return paquete;
};

void delete_package(t_paquete* paquete)
{
	free(paquete->buffer->stream);
	free(paquete->buffer);
	free(paquete);
}

int socket_initialize(char *puerto){
    struct addrinfo hints, *servinfo;

	memset(&hints, 0, sizeof(hints));
	hints.ai_family = AF_UNSPEC;
	hints.ai_socktype = SOCK_STREAM;
	hints.ai_flags = AI_PASSIVE;

	getaddrinfo(NULL, puerto, &hints, &servinfo);

	// Creamos el socket de escucha del servidor
	int socket_servidor = socket(servinfo->ai_family,
						servinfo->ai_socktype,
						servinfo->ai_protocol);
	// Asociamos el socket a un puerto
	bind(socket_servidor, servinfo->ai_addr, servinfo->ai_addrlen);
	// Escuchamos las conexiones entrantes
	listen(socket_servidor, SOMAXCONN);

	freeaddrinfo(servinfo);
	printf("Listo para escuchar a mi cliente\n");

	return socket_servidor;
}

int socket_initialize_connect(char *ip,char *puerto)
{

	struct addrinfo hints, *server_info;

	memset(&hints, 0, sizeof(hints));
	hints.ai_family = AF_UNSPEC;
	hints.ai_socktype = SOCK_STREAM;
	hints.ai_flags = AI_PASSIVE;

	getaddrinfo(ip, puerto, &hints, &server_info);

	// Creamos el socket de escucha del servidor
	int socket_servidor = socket(server_info->ai_family,
						server_info->ai_socktype,
						server_info->ai_protocol);
	// Ahora que tenemos el socket, vamos a conectarlo
 	int connection = connect(socket_servidor, server_info->ai_addr, server_info->ai_addrlen);
	if ( connection == (-1) ){
		printf("\nERROR AL CONECTARME CON EL SERVIDOR PROPORCIONADO");
	}
	printf("\nConexion establecida en el puerto %s con la ip %s \n",puerto,ip);
	freeaddrinfo(server_info);

	return socket_servidor;
}

void socket_send_message(char* mensaje, int socket_cliente)
{
	t_paquete* paquete = malloc(sizeof(t_paquete));

	paquete->codigo_operacion = MENSAJE;
	paquete->buffer = malloc(sizeof(t_buffer));
	paquete->buffer->size = strlen(mensaje) + 1;
	paquete->buffer->stream = malloc(paquete->buffer->size);
	memcpy(paquete->buffer->stream, mensaje, paquete->buffer->size);

	int bytes = paquete->buffer->size + 2*sizeof(int);

	void* a_enviar = serialize_package(paquete, bytes);
	printf("\nEnviando mensaje a servidor \n");
	int mess_send = send(socket_cliente, a_enviar, bytes, 0);

	if(mess_send == (-1) ) {
		printf("\nERROR AL ENVIAR MENSAJE");
	}

	free(a_enviar);
	delete_package(paquete);
}

void socket_send_package(t_paquete* paquete, int socket_cliente)
{
	int bytes = paquete->buffer->size + 2*sizeof(int);

	void* a_enviar = serialize_package(paquete, bytes);
	printf("\nEnviando paquete al servidor \n");
	int mess_send = send(socket_cliente, a_enviar, bytes, 0);

	if(mess_send == (-1) ) {
		printf("\nERROR AL ENVIAR MENSAJE");
	}
	
	free(a_enviar);
	delete_package(paquete);
}

void socket_end(int socket_cliente){
	close(socket_cliente);
}

void* serialize_recv_buffer(int* size, int socket_cliente)
{
	void * buffer;

	recv(socket_cliente, size, sizeof(int), MSG_WAITALL);
	buffer = malloc(*size);
	recv(socket_cliente, buffer, *size, MSG_WAITALL);

	return buffer;
}

t_list* serialize_recv_package(int socket_cliente)
{
    int size;
    int desplazamiento = 0;
    void *buffer;
    t_list *valores = list_create();
    int tamanio;

    buffer = serialize_recv_buffer(&size, socket_cliente);
    while (desplazamiento < size)
    {
        memcpy(&tamanio, buffer + desplazamiento, sizeof(int));
        desplazamiento += sizeof(int);
        char *valor = malloc(tamanio + 1); // Add one byte for null terminator
        memcpy(valor, buffer + desplazamiento, tamanio);
        valor[tamanio] = '\0'; // Add null terminator
        desplazamiento += tamanio;
        list_add(valores, valor);
    }
    free(buffer);
    return valores;
}

int socket_accept(int socket_servidor)
{

	int socket_cliente = accept(socket_servidor, NULL, NULL);
	if(socket_cliente == (-1)){
		printf( "Error de conexion al servidor! \n ");
	}
	
	if( socket_cliente ){
		printf( "Se conecto alguien al servidor! \n ");
	}

	return socket_cliente;
}

int socket_recv_operation(int socket_cliente)
{
	int cod_op;
	if(recv(socket_cliente, &cod_op, sizeof(int), MSG_WAITALL) > 0)
		return cod_op;
	else
	{
		close(socket_cliente);
		return -1;
	}
}

void socket_recv_message(int socket_cliente)
{
	int size;
	char* buffer = serialize_recv_buffer(&size, socket_cliente);
	printf( "Me llego el mensaje %s \n", buffer);
	free(buffer);
}

t_list* socket_recv_package(int socket_cliente){
	return serialize_recv_package(socket_cliente);
}

int conect_modules(t_config* config,t_log* logger,char* modulo){

	char* puerto = malloc(strlen("PUERTO_") + strlen(modulo) + 1 );
	sprintf(puerto,"PUERTO_%s",modulo);
	char* ip = malloc(strlen("IP_") + strlen(modulo) + 1);
	sprintf(ip,"IP_%s",modulo);

    char* puerto_modulo = config_get_string_value(config, puerto);
    char* ip_modulo= config_get_string_value(config,ip);

    int conexion_modulo = socket_initialize_connect(ip_modulo,puerto_modulo);

	printf("Conexión con módulo %s establecida.\n",modulo);
	char* mensaje = "prueba";
	socket_send_message(mensaje,conexion_modulo);
	
	return conexion_modulo;
}

int receive_modules(t_log* logger,t_config* config){
	//Obtenemos el puerto con el que escucharemos conexiones
	char* puerto = config_get_string_value(config, "PUERTO_ESCUCHA");

    log_info(logger,"El valor del puerto es %s \n",puerto);    

    //Inicializo el socket en el puerto cargado por la config
    int server_fd = socket_initialize(puerto);
    log_info(logger,"SOCKET INICIALIZADO");    
    //Pongo el socket en modo de aceptar las escuchas
	//int cliente_fd = socket_accept(server_fd);

	

	return socket_accept(server_fd);
}