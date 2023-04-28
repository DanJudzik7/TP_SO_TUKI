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
        case F_TRUNCATE:
        case F_SEEK:
        case CREATE_SEGMENT:
        case I_O:
        case WAIT:
        case SIGNAL:
        case F_OPEN:
        case F_CLOSE:
        case DELETE_SEGMENT:
        case EXIT:
        case YIELD:
        default:
            // INSERTAR ERROR
            break;
        }
}

void set(execution_context* execution_context,t_list* instruction){

     //almaceno el nombre del registro y el valor 
     char* register_name = list_get( instruction , 1 );
     //lo paso a un int con atoi el valor proporcionado en la lista
     int value = atoi( list_get( instruction , 2) );

     //comparo el registro y asigno el valor correspondiente

    cpu_register *registers = &execution_context->cpu_register;

     if(strcmp(register_name, "AX") == 0){

          registers->acumulator = value;
          
     } else if(strcmp(register_name, "BX") == 0){

         registers->register_base = value;
     } else if(strcmp(register_name, "CX") == 0){

         registers->counter = value;
     } else if(strcmp(register_name, "DX") == 0){

         registers->register_data = value;
     }
     
     printf("El valor de %s es %i \n",register_name,value);
}
