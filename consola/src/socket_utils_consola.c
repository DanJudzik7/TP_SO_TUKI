#include "socket_utils_consola.h"


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
	printf("\nEnviando mensaje a kernel");
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
	printf("\nEnviando mensaje a kernel");
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


