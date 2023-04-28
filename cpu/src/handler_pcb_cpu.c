#include "handler_pcb_cpu.h"

void instruction_cycle(pcb* pcb){

        // devuelve el puntero de la lista a ejecutar dentro de la lista de instrucciones
        t_list* next_instruction = fetch(pcb); 

        if (next_instruction != NULL) {
            op_code COD_OP = decode(next_instruction);
            execute(COD_OP, next_instruction, pcb);
            pcb->execution_context->program_counter++;
        } else {
        // No hay más instrucciones, manejar errores
        }
           
           //En cuyo caso que tengamos que hacer while hasta el manejo de una excepcion meter dentro del while
    /*while (pcb-> execution_context-> program_counter < list_size (pcb->execution_context -> instructions))*/

    
}
// obtiene la instruccion(lista) actual a ejecutar
t_list* fetch(pcb* pcb){
    return list_get( *(pcb->execution_context -> instructions) , pcb->execution_context->program_counter );
}

// Esta etapa consiste en interpretar qué instrucción es la que se va a ejecutar 
// TODO:: y si la misma requiere de una traducción de dirección lógica a dirección física.    <-------- hacer la traduccion de direcciones en decode.
op_code decode(t_list* next_instruction){    
    return (op_code) list_get(next_instruction , 0);                                           
}

void execute(op_code COD_OP,t_list* instruction,pcb* pcb){  

    switch (COD_OP){
        case SET:
            set(pcb->execution_context, instruction);
            break;
        case MOV_IN:
        case MOV_OUT:
        case I_O:
        case F_OPEN:
        case F_CLOSE:
        case F_SEEK:
        case F_READ:
        case F_WRITE:
        case F_TRUNCATE:
        case WAIT:
        case SIGNAL:
        case CREATE_SEGMENT:
        case DELETE_SEGMENT:
            printf("Implementar funcion");
            break;
        case YIELD:
            yield(pcb);
            break;
        case EXIT:
            exit(pcb);
            break;
        default:
            // INSERTAR ERROR
            break;
        }
}
