#include "socket_utils_kernel.h"



int socket_initialize(char *puerto)
{

	struct addrinfo hints, *servinfo;

	memset(&hints, 0, sizeof(hints));
	hints.ai_family = AF_UNSPEC;
	hints.ai_socktype = SOCK_STREAM;
	hints.ai_flags = AI_PASSIVE;

	int address_info = getaddrinfo(NULL, puerto, &hints, &servinfo);
	if( address_info == (-1)){
		printf("ERROR AL OBTENER LOS DATOS PARA INICIALIZAR EL SOCKET");
	}

	// Creamos el socket de escucha del servidor
	int socket_servidor = socket(servinfo->ai_family,
						servinfo->ai_socktype,
						servinfo->ai_protocol);
	// Asociamos el socket a un puerto
	bind(socket_servidor, servinfo->ai_addr, servinfo->ai_addrlen);
	// Escuchamos las conexiones entrantes
	listen(socket_servidor, SOMAXCONN);   //SOMAXCONN DETERMINA LA CANTIDAD MAXICA DE CONEXIONES EN COLA, MANEJAR ESTO CREO QUE SE PIDE EN EL TP

	freeaddrinfo(servinfo);
	printf( "Servidor Kernel levantado listo para escuchar un cliente \n");

	return socket_servidor;
}

int socket_accept(int socket_servidor)
{

	int socket_cliente = accept(socket_servidor, NULL, NULL);
	if(socket_cliente == (-1)){
		printf( "Error de conexion al kernel! \n ");
	}
	
	if( socket_cliente ){
		printf( "Se conecto alguien al kernel! \n ");
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

void* socket_recv_buffer(int* size, int socket_cliente)
{
	void * buffer;

	recv(socket_cliente, size, sizeof(int), MSG_WAITALL);
	buffer = malloc(*size);
	recv(socket_cliente, buffer, *size, MSG_WAITALL);

	return buffer;
}

void socket_recv_message(int socket_cliente)
{

	int size;
	char* buffer = socket_recv_buffer(&size, socket_cliente);
	printf( "Me llego el mensaje %s \n", buffer);
	free(buffer);
}