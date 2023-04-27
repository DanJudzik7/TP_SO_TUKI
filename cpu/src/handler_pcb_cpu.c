#include "handler_pcb_cpu.h"

void execute(execution_context* execution_context){
    printf("hola, entre %i ", execution_context->program_counter);
    

   while (execution_context->program_counter < list_size (execution_context -> instructions)){
        printf("%s",list_get(list_get(execution_context -> instructions,execution_context->program_counter),0));
        char* COD_OP = list_get(list_get(execution_context -> instructions,execution_context->program_counter),0);
        
        if(strcmp(COD_OP,"SET") == 0){
            //almaceno el nombre del registro y el valor 
            char* register_name = list_get(list_get(execution_context -> instructions,execution_context->program_counter),1);
            //lo paso a un int
            int value = atoi(list_get(list_get(execution_context -> instructions,execution_context->program_counter),2));

            //comparo el registro y asigno el valor correspondiente
            if(strcmp(register_name, "AX") == 0){
                 execution_context->cpu_register.acumulator = value;
            } else if(strcmp(register_name, "BX") == 0){
                execution_context->cpu_register.register_base = value;
            } else if(strcmp(register_name, "CX") == 0){
                execution_context->cpu_register.counter = value;
            } else if(strcmp(register_name, "DX") == 0){
                execution_context->cpu_register.register_data = value;
            }
        } else if(strcmp(COD_OP,"YIELD") == 0){
            // devuelve el contexto de ejecucion al kernel
        }else if(strcmp(COD_OP,"EXIT") == 0){
            //Finaliza el programa
        }
        
        execution_context->program_counter ++;
   }
   
}