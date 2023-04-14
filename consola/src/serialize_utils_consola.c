#include "socket_utils_consola.h"
#include "consola.h"

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

void add_to_package(t_paquete* paquete, void* valor, int tamanio) //mandamos estructuras individuales de cada instruccion
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

t_paquete* create_package(int cod_op){ // 1 creamos paquetes individuales
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



void socket_send_0_params(char* instruccion,int spaces,int socket_cliente){
	//EMPIEZA ESTRUCTURA INSTRUCCION
	t_instruccion_package_0 inst;
	inst.instruccion = malloc(strlen(instruccion)+1);
	inst.instruccion_long = (strlen(instruccion)+1);
	strcpy(inst.instruccion, instruccion);
		//cod_op es la cantidad de parametros

	//EMPIEZA BUFFER
	t_buffer* buffer = malloc(sizeof(t_buffer));
	buffer->size = inst.instruccion_long + sizeof(uint32_t);
	void* stream = malloc(buffer->size);
	int offset = 0;
	memcpy(stream + offset,&inst.instruccion,inst.instruccion_long);
	buffer->stream = stream;

	//EMPIEZA PAQUETE
	t_paquete* paquete = malloc(sizeof(t_paquete));
	paquete->codigo_operacion = spaces; 
	paquete->buffer = buffer;

	void* a_enviar = malloc(buffer->size + sizeof(uint8_t) + sizeof(uint32_t));//VER DE DONDE SALE ESE UINT32_T
	offset = 0;
	memcpy(a_enviar + offset, &(paquete->codigo_operacion), sizeof(uint8_t));

	
    printf("spaces: %i, INSTRUCCION: %s",spaces,instruccion);
	printf("codigo: %i",paquete->codigo_operacion);
	send(socket_cliente, a_enviar, buffer->size + sizeof(uint8_t) + sizeof(uint32_t), 0);
}
