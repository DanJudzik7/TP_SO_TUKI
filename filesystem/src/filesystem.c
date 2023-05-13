#include "filesystem.h"

int main(int argc, char** argv) {
	t_log* logger = start_logger("filesystem");
	t_config* config = start_config("filesystem");
	log_warning(logger, "Iniciando el filesystem");

	int cliente_fd = receive_modules(logger, config);

	// TODO: En el primer recv llega basura no se porque
	socket_receive_buffer(cliente_fd, 17);

	int socket_memory = connect_module(config, logger, "MEMORIA");
}