#include "handlers.h"
pthread_mutex_t mutex_compact;
pthread_mutex_t mutex_write;

// Manejo los recive con cada una de estas funciones
void handle_fs(int socket_fs,memory_structure* memory_structure){
    while (1)
    {
        t_package* package_fs = socket_receive(socket_fs);
        if (package_fs == NULL) {
                printf("El cliente se desconectó\n");
                exit(1);
                break;
        }

        segment_read_write* segment_rw= deserialize_segment_read_write(package_fs);

        switch (package_fs->type){
        case F_READ:
            sleep(memory_shared.mem_delay);
            char* buffer = read_memory(segment_rw->s_id, segment_rw->offset, segment_rw->size, memory_structure, segment_rw->pid);
            if(buffer == NULL){
                // devolver seg_fault
                send(socket_fs,package_new(SEG_FAULT),sizeof(t_package),0);
            } else {
                // devolver buffer
                send(socket_fs,buffer,sizeof(buffer),0);
            }
            break;
        case F_WRITE:       
            sleep(memory_shared.mem_delay);
            pthread_mutex_lock(&mutex_write);
            if(write_memory(segment_rw->s_id,segment_rw->offset, segment_rw->size, segment_rw->buffer, memory_structure, segment_rw->pid)) {
                send(socket_fs,package_new(OK_INSTRUCTION),sizeof(t_package),0);
            } else {
                send(socket_fs,package_new(SEG_FAULT),sizeof(t_package),0);
            }
            pthread_mutex_unlock(&mutex_write);
            break;   

        default:
            log_error(memory_config.logger,"El proceso recibió algo indebido, finalizando modulo");
            exit(1);
            break;
        }
   }
}

void handle_cpu(int socket_cpu,memory_structure* memory_structure){
    while (1)
    {
    t_package* package_cpu = socket_receive(socket_cpu);
    if (package_cpu == NULL) {
			printf("El cliente se desconectó\n");
			exit(1);
	}

    segment_read_write* segment_rw= deserialize_segment_read_write(package_cpu);

    switch (package_cpu->type)
    {
    case F_READ:
        sleep(memory_shared.mem_delay);
        char* buffer = read_memory(segment_rw->s_id, segment_rw->offset, segment_rw->size, memory_structure, segment_rw->pid);
        if(buffer == NULL){
            // devolver seg_fault
            send(socket_cpu,package_new(SEG_FAULT),sizeof(t_package),0);
        } else {
            // devolver buffer
            send(socket_cpu,buffer,sizeof(buffer),0);
         }
        break;
    case F_WRITE:       
        sleep(memory_shared.mem_delay);
        pthread_mutex_lock(&mutex_write);
        if(write_memory(segment_rw->s_id,segment_rw->offset, segment_rw->size, segment_rw->buffer, memory_structure, segment_rw->pid)) {
            // devolver ok
            send(socket_cpu,package_new(OK_INSTRUCTION),sizeof(t_package),0);
        } else {
            // devolver seg_fault
            send(socket_cpu,package_new(SEG_FAULT),sizeof(t_package),0);

        }
        pthread_mutex_unlock(&mutex_write);
        break;   
     
    default:
        log_error(memory_config.logger,"El proceso recibió algo indebido, finalizando modulo");
        exit(1);
        break;
    }
    }
}

void handle_kernel(int socket_kernel,memory_structure* memory_structure){
    while (1)
    {
    t_package* package_kernel = socket_receive(socket_kernel);
    if (package_kernel == NULL) {
			printf("El cliente se desconectó\n");
			exit(1);
	}

    segment_table* sg = deserialize_segment_table(package_kernel);
  
    char* process_id;
    sprintf(process_id, "%d", sg->pid);

    switch (package_kernel->type)
    {
    case CREATE_PROCESS_MEMORY:
    // Creo la tabla de segmentos y la devuevlo al kernel cuando crea un proceso
    t_list* segment_table = create_sg_table(memory_structure, sg->pid);
    log_info(memory_config.logger,"Creación de Proceso PID: %s",process_id);
    send(socket_kernel,segment_table,sizeof(segment_table),0); 
        break;
    case END_PROCCESS_MEMORY:
        remove_sg_table(memory_structure, sg->pid);
        log_info(memory_config.logger,"Eliminación de Proceso PID: %s",process_id);
        break;
    case CREATE_SEGMENT:
        int flag = add_segment(memory_structure, sg->pid, sg->size_data_segment, sg->s_id);
        switch (flag)
        {
        case 0:
            // Devuelvo al kernel la base del segmento
            break;
        case 1:
            // Devuelvo solicitud de compactacion
            break;
        case 2:
            // Devuelvo no hay espacio suficiente
            break;
        
        default:
            break;
        }
        break;
    case DELETE_SEGMENT:
        delete_segment(memory_structure, sg->pid, sg->s_id);
        break;
    case COMPACT_MEMORY:
        log_info(memory_config.logger,"Solicitud de compactacion");
        sleep(memory_shared.com_delay);
        pthread_mutex_lock(&mutex_compact);
        compact_memory(memory_structure);
        pthread_mutex_unlock(&mutex_compact);
        break;

    default:
        log_error(memory_config.logger,"El proceso recibió algo indebido, finalizando modulo");
        exit(1);
        break;
    }
    }
}