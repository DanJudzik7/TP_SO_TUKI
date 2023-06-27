#include "handlers.h"

// Manejo los recive con cada una de estas funciones
void handle_fs(int socket_fs,memory_structure* memory_structure){
    t_package* fs = socket_receive(socket_fs);
    if (fs == NULL) {
			printf("El cliente se desconectó\n");
			exit(1);
	}

    int s_id;
    int offset;
    int size;
    char* buffer;
    int pid;
    switch (fs->type)
    {
    //case READ:
        sleep(memory_shared.mem_delay);
        buffer = read_memory(s_id,offset,size,memory_structure,pid);
        if(buffer == NULL){
            // devolver seg_fault
        } else {
            // devolver buffer
        }
        break;
    //case WRITE:       
        sleep(memory_shared.mem_delay);
        if(write_memory(s_id,offset,size,buffer,memory_structure,pid)) {
              // devolver ok
        } else {
              // devolver seg_fault
        }
        break;   

    default:
        log_error(memory_config.logger,"El proceso recibió algo indebido, finalizando modulo");
        exit(1);
        break;
    }

}

void handle_cpu(int socket_cpu,memory_structure* memory_structure){
    t_package* cpu = socket_receive(socket_cpu);
    if (cpu == NULL) {
			printf("El cliente se desconectó\n");
			exit(1);
	}
    int s_id;
    int offset;
    int size;
    char* buffer;
    int pid;
    switch (cpu->type)
    {
        
    //case READ:
        sleep(memory_shared.mem_delay);
        buffer = read_memory(s_id,offset,size,memory_structure,pid);
        if(buffer == NULL){
            // devolver seg_fault
        } else {
            // devolver buffer
        }
        break;
    //case WRITE:       
        sleep(memory_shared.mem_delay);
       if(write_memory(s_id,offset,size,buffer,memory_structure,pid)) {
              // devolver ok
        } else {
              // devolver seg_fault
        }
        break;   
     
    default:
        log_error(memory_config.logger,"El proceso recibió algo indebido, finalizando modulo");
        exit(1);
        break;
    }
}

void handle_kernel(int socket_kernel,memory_structure* memory_structure){
    t_package* kernel = socket_receive(socket_kernel);
    if (kernel == NULL) {
			printf("El cliente se desconectó\n");
			exit(1);
	}
    int pid = 1;// HARDCODEADISIMO. ESTE PID LO DEBERIA RECIBIR DEL KERNEL
    int size = 1234;// HARDCODEADISIMO. ESTE SIZE LO DEBERIA RECIBIR DEL KERNEL
    int s_id = 1;// HARDCODEADISIMO. ESTE S_ID LO DEBERIA RECIBIR DEL KERNEL
    
    char* process_id;
    sprintf(process_id, "id%d", pid);

    switch (kernel->type)
    {
    //case NEW_PROCCESS:
    // Creo la tabla de segmentos y la devuevlo al kernel cuando crea un proceso
    t_list* segment_table = create_sg_table(memory_structure,process_id);
    log_info(memory_config.logger,"Creación de Proceso PID: %s",process_id);
    send(socket_kernel,segment_table,sizeof(segment_table),0); 
        break;
    //case END_PROCCESS:
    // Elimino la tabla de segmentos cuando termina un proceso
        remove_sg_table(memory_structure,process_id);
        log_info(memory_config.logger,"Eliminación de Proceso PID: %s",process_id);
        break;
    case CREATE_SEGMENT:
        add_segment(memory_structure,process_id,size,s_id);
        break;
    case DELETE_SEGMENT:
        /* code */
        break;
    //case COMPACT:
        log_info(memory_config.logger,"Solicitud de compactacion");
        sleep(memory_shared.com_delay);
        break;

    default:
        log_error(memory_config.logger,"El proceso recibió algo indebido, finalizando modulo");
        exit(1);
        break;
    }
}