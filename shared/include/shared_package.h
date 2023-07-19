#ifndef SHARED_PACKAGE_H
#define SHARED_PACKAGE_H

#include "shared_utils.h"
#include "shared_serializer.h"

// Librería de paquetes
// Se encarga del manejo de bajo nivel de paquetes de datos
// Permite interpretar tipos recursivos con facilidad

typedef struct t_package {
	uint64_t size;
	int32_t type;
	void* buffer;
} t_package;

typedef enum t_package_type {
    SERIALIZED,             // 0
    MESSAGE_OK,             // 1
    MESSAGE_FLAW,           // 2
    MESSAGE_DONE,           // 3
    INSTRUCTIONS,           // 4
    EXECUTION_CONTEXT       // 5
} t_package_type;

// Crea y retorna un paquete con el código de operación especificado.
t_package* package_new(int32_t type);

// Crea un paquete de modo key-value que contiene un valor o buffer
t_package* package_new_dict(int32_t key, void* value, uint64_t* value_size);

// Crea un paquete de modo key-value que contiene otro paquete
t_package* package_new_nested(int32_t key, t_package* value);

// Inserta un paquete dentro de otro
void package_nest(t_package* package, t_package* nested);

// Agrega un valor con un tamaño específico al paquete especificado
void package_add(t_package* package, void* value, uint64_t* value_size);

// Agrega texto plano a un paquete
void package_write(t_package* package, char* string);

// Deserializa un paquete
t_package* package_decode(void* source, uint64_t* offset);

// Comprueba si en una lista de paquetes aún hay más paquetes a continuación
bool package_decode_isset(t_package* package, uint64_t offset);

// Deserializa un string
char* package_decode_string(void* source, uint64_t* offset);

// Deserializa un string a un espacio en memoria existente
void package_decode_buffer(void* source, void* dest, uint64_t* offset);

// Destruye eliminando de la memoria el paquete especificado
void package_destroy(t_package* package);

#endif