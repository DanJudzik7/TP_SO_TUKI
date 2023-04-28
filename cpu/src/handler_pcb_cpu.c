#include "handler_pcb_cpu.h"

void execute(execution_context* execution_context,int PID){
    

    while (execution_context->program_counter < list_size (execution_context -> instructions)){
        printf("hola, entre %i \n", execution_context->program_counter);
        printf("%s\n",list_get(list_get(execution_context -> instructions,execution_context->program_counter),0));
        char* codigo = list_get(list_get(execution_context -> instructions,execution_context->program_counter),0);
        op_code COD_OP = return_opcode(codigo);

        switch (COD_OP){
        case SET:
            set(execution_context);
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
            break;
        }
        
       execution_context->program_counter ++;
   }
   
}

void set(execution_context* execution_context){
     //almaceno el nombre del registro y el valor 
     char* register_name = list_get(list_get(execution_context -> instructions,execution_context->program_counter),1);
     //lo paso a un int con atoi
     int value = atoi(list_get(list_get(execution_context -> instructions,execution_context->program_counter),2));

     //comparo el registro y asigno el valor correspondiente
     if(strcmp(register_name, "AX") == 0){
          execution_context->cpu_register.acumulator = value;
          printf("El valor de %s es %i \n",register_name,value);
     } else if(strcmp(register_name, "BX") == 0){
         execution_context->cpu_register.register_base = value;
     } else if(strcmp(register_name, "CX") == 0){
         execution_context->cpu_register.counter = value;
     } else if(strcmp(register_name, "DX") == 0){
         execution_context->cpu_register.register_data = value;
     }
     printf("ejecuto \n");
}

