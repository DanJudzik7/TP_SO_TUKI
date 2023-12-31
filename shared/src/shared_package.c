#include "shared_package.h"

t_package* package_new(int32_t type) {
	t_package* package = s_malloc(sizeof(t_package));
	package->type = type;
	package->size = 0;
	package->buffer = NULL;
	return package;
}

t_package* package_new_dict(int32_t key, void* value, uint64_t* value_size) {
	t_package* package = package_new(key);
	package_add(package, value, value_size);
	return package;
}

t_package* package_new_nested(int32_t key, t_package* value) {
	t_package* package = package_new(key);
	package_nest(package, value);
	return package;
}

void package_nest(t_package* package, t_package* nested) {
	package_close(nested); // Convierte el package al tipo Serialized
	uint64_t offset = package->size;
	package->size += nested->size;
	package->buffer = realloc(package->buffer, package->size);
	memcpy(package->buffer + offset, nested->buffer, nested->size);
	package_destroy(nested);
}

void package_add(t_package* package, void* value, uint64_t* value_size) {
    uint64_t offset = package->size;
    package->size += sizeof(uint64_t) + *value_size;
    package->buffer = realloc(package->buffer, package->size);
    memcpy(package->buffer + offset, value_size, sizeof(uint64_t));
    memcpy(package->buffer + offset + sizeof(uint64_t), value, *value_size);
}

void package_write(t_package* package, char* value) {
	uint64_t size = strlen(value) + 1;
	char* new_value = s_malloc(size);
	memcpy(new_value, value, size);
	new_value[size - 1] = '\0';
	package_add(package, new_value, &size);
	free(new_value);
}

t_package* package_decode(void* source, uint64_t* offset) {
	bool discarded_offset = offset == NULL;
	if (discarded_offset) {
		offset = s_malloc(sizeof(uint64_t));
		*offset = 0;
	}
	t_package* package = s_malloc(sizeof(t_package));
	memcpy(&(package->size), source + *offset, sizeof(uint64_t));
	*offset += sizeof(uint64_t);
	memcpy(&(package->type), source + *offset, sizeof(int32_t));
	*offset += sizeof(int32_t);
	package->buffer = s_malloc(package->size);
	memcpy(package->buffer, source + *offset, package->size);
	*offset += package->size;
	if (discarded_offset) free(offset);
	return package;
}

bool package_decode_isset(t_package* package, uint64_t offset) {
	return offset < package->size;
}

char* package_decode_string(void* buffer, uint64_t* offset) {
	bool discarded_offset = offset == NULL;
	if (discarded_offset) {
		offset = s_malloc(sizeof(uint64_t));
		*offset = 0;
	}
	uint64_t *size = s_malloc(sizeof(uint64_t));
	memcpy(size, buffer + *offset, sizeof(uint64_t));
	*offset += sizeof(uint64_t);
	char *str = s_malloc(*size + 1);
	memcpy(str, buffer + *offset, *size);
	str[*size] = '\0';
	*offset += *size;
	free(size);
	if (discarded_offset) free(offset);
	return str;
}

void package_decode_buffer(void* buffer, void* dest, uint64_t* offset) {
	bool discarded_offset = offset == NULL;
	if (discarded_offset) {
		offset = s_malloc(sizeof(uint64_t));
		*offset = 0;
	}
	uint64_t *size = s_malloc(sizeof(uint64_t));
	memcpy(size, buffer + *offset, sizeof(uint64_t));
	*offset += sizeof(uint64_t);
	memcpy(dest, buffer + *offset, *size);
	*offset += *size;
	free(size);
	if (discarded_offset) free(offset);
}

void package_close(t_package* package) {
	uint64_t package_size = sizeof(uint64_t) + sizeof(int32_t) + package->size;
	void* stream = s_malloc(package_size);
	uint64_t offset = 0;
	memcpy(stream + offset, &(package->size), sizeof(uint64_t));
	offset += sizeof(uint64_t);
	memcpy(stream + offset, &(package->type), sizeof(int32_t));
	offset += sizeof(int32_t);
	memcpy(stream + offset, package->buffer, package->size);
	package->type = SERIALIZED;
	package->size = package_size;
	free(package->buffer);
	package->buffer = stream;
}

void package_destroy(t_package* package) {
	free(package->buffer);
	package->buffer = NULL;
	free(package);
	package = NULL;
}