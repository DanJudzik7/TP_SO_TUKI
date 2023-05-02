#include "handler_pcb.h"

void create_pcb(t_console_init* ci) {
	t_log* logger = ci->logger;
	t_pcb* pcb = malloc(sizeof(t_pcb));
	pcb->state_pcb = NEW;
	pcb->pid = 2000 + ci->console;
	pcb->aprox_burst_time = ci->default_burst_time;
	pcb->execution_context = malloc(sizeof(execution_context));
	pcb->execution_context->instructions = queue_create();
	pcb->execution_context->program_counter = 0;
	log_info(logger, "Proceso %d creado en NEW", pcb->pid);
	queue_push(ci->global_pcb, pcb);
	handle_incoming_instructions(pcb->execution_context->instructions, ci->console, logger);
	free(ci);
}

int aprox_burst_time(int value_aprox_time) {
	return value_aprox_time;
}