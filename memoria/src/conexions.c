#include "conexions.h"

void listen_modules(int socket_memory){
    pthread_t thread_fs;
    pthread_create(&thread_fs, NULL, (void*)listen_fs, socket_memory);
	pthread_join(thread_fs,NULL);

    pthread_t thread_cpu;
    pthread_create(&thread_cpu, NULL, (void*)listen_cpu, socket_memory);
	pthread_join(thread_cpu,NULL);
    
    pthread_t thread_k;
    pthread_create(&thread_k, NULL, (void*)listen_kernel, socket_memory);
	pthread_join(thread_k,NULL);
}

void listen_fs(int socket_memory){
    int socket_fs = socket_accept(socket_memory);
    if (socket_fs == -1) {
			log_warning(memory_config.logger, "Hubo un error aceptando la conexión");
			exit(-1);
	}else {
        	log_warning(memory_config.logger, "Se conecto el kernel en el puerto %d",socket_fs);
    }
    handle_fs(socket_fs);
}

void listen_cpu(int socket_memory){
    int socket_cpu = socket_accept(socket_memory);
    if (socket_cpu == -1) {
			log_warning(memory_config.logger, "Hubo un error aceptando la conexión");
			exit(-1);
	}else {
        	log_warning(memory_config.logger, "Se conecto el kernel en el puerto %d",socket_cpu);
    }
    handle_cpu(socket_cpu);
}



void listen_kernel(int socket_memory){
    pthread_t thread;
    int socket_kernel = socket_accept(socket_memory);
    if (socket_kernel == -1) {
			log_warning(memory_config.logger, "Hubo un error aceptando la conexión");
			exit(-1);
	}else {
        	log_warning(memory_config.logger, "Se conecto el kernel en el puerto %d",socket_kernel);
    }
    handle_kernel(socket_kernel);
}