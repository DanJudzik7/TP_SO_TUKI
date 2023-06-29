#include "conexions.h"


void listen_modules(int socket_memory,memory_structure* memory_structure){

    int fs = listen_fs(socket_memory);
    int cpu = listen_cpu(socket_memory);
    int kernel = listen_kernel(socket_memory);

    pthread_t thread_filesystem;
    pthread_create(&thread_filesystem, NULL, (void*)handle_fs, socket_memory);
	pthread_detach(thread_filesystem);

    pthread_t thread_cpu;
    pthread_create(&thread_cpu, NULL, (void*)handle_cpu, socket_memory);
	pthread_detach(thread_cpu);
    
    pthread_t thread_k;
    pthread_create(&thread_k, NULL, (void*)handle_kernel, socket_memory);
	pthread_detach(thread_k);
   
}

int listen_fs(int socket_memory){
    int socket_fs = socket_accept(socket_memory);
    if (socket_fs == -1) {
			log_warning(memory_config.logger, "Hubo un error aceptando la conexión");
			exit(-1);
	}else {
        	log_warning(memory_config.logger, "Se conecto el File System en el puerto %d",socket_fs);
    }
    return socket_fs;
}

int listen_cpu(int socket_memory){
    int socket_cpu = socket_accept(socket_memory);
    if (socket_cpu == -1) {
			log_warning(memory_config.logger, "Hubo un error aceptando la conexión");
			exit(-1);
	}else {
        	log_warning(memory_config.logger, "Se conecto la CPU en el puerto %d",socket_cpu);
    }
    return socket_cpu;
}

int listen_kernel(int socket_memory){
    int socket_kernel = socket_accept(socket_memory);
    if (socket_kernel == -1) {
			log_warning(memory_config.logger, "Hubo un error aceptando la conexión");
			exit(-1);
	}else {
        	log_warning(memory_config.logger, "Se conecto el Kernel en el puerto %d",socket_kernel);
    }
    return socket_kernel;
}
