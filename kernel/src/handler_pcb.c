#include "handler_pcb.h"

void listen_consoles(t_global_config_kernel* gck) {
	while (1) {
		pthread_t thread;
		int console_socket = socket_accept(gck->connection_kernel);
		if (console_socket == -1) {
			log_warning(gck->logger, "Hubo un error aceptando la conexión");
			continue;
		}
		// Se crea el PCB y se agrega a la cola NEW
		log_info(gck->logger, "El proceso %d se creó en NEW", console_socket);
		t_pcb* pcb = pcb_new(console_socket, gck->default_burst_time);
		queue_push(gck->new_pcbs, pcb);
		// Se crea un thread para escuchar las instrucciones
		pthread_create(&thread, NULL, (void*)handle_incoming_instructions, pcb);
		pthread_detach(thread);
		// Se planifica el proceso
		long_term_schedule(gck);
	}
}

t_pcb* pcb_new(int pid, int burst_time) {
	// La inicialización se hace de forma segura en memoria (con memset)
	t_pcb* pcb = s_malloc(sizeof(t_pcb));
	pcb->state = NEW;
	pcb->pid = pid;
	pcb->aprox_burst_time = burst_time;
	pcb->last_ready_time = time(NULL);
	pcb->files = list_create();
	pcb->execution_context = s_malloc(sizeof(execution_context));
	pcb->execution_context->instructions = queue_create();
	pcb->execution_context->program_counter = 0;
	pcb->execution_context->updated_state = NEW;
	pcb->execution_context->cpu_register = s_malloc(sizeof(cpu_register));
	memset(pcb->execution_context->cpu_register, 0, sizeof(cpu_register));
	pcb->execution_context->segment_table = s_malloc(sizeof(segment_table));
	memset(pcb->execution_context->segment_table, 0, sizeof(segment_table));
	return pcb;
}

void pcb_destroy(t_pcb* pcb) {
	list_destroy_and_destroy_elements(pcb->files, (void*)delete_file);
	queue_destroy_and_destroy_elements(pcb->execution_context->instructions, (void*)delete_instruction);
	free(pcb->execution_context->cpu_register);
	pcb->execution_context->cpu_register = NULL;
	free(pcb->execution_context->segment_table);
	pcb->execution_context->segment_table = NULL;
	free(pcb->execution_context);
	pcb->execution_context = NULL;
	free(pcb);
	pcb = NULL;
}

void delete_file(t_file* file) {
	// Implementar
}

void delete_instruction(t_instruction* instruction) {
	// Implementar
}