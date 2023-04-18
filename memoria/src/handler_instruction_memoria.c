#include "handler_instruction_memoria.h"

int instruction_handler_memoria(int cliente_fd){

    t_list* payload;
    t_paquete* paquete;
    
	while (1) {

		int cod_op = socket_recv_operation(cliente_fd);
        printf("\nEl codigo de operacion es -> %i\n",cod_op);

		switch (cod_op) {
            case CREATE_SEGMENT:
                printf("RECIBIMOS UNA INSTRUCCION DE CREAR SEGMENTO DE MEMORIA");
                op_code_reception result_creation_segment = create_memory_segment();
                paquete = create_package( result_creation_segment ); 
                socket_send_package(paquete, cliente_fd);

                break;
            case DELETE_SEGMENT:
                printf("RECIBIMOS UNA INSTRUCCION DE ELIMINAR UN SEGMENTO DE MEMORIA");
                payload = socket_recv_package(cliente_fd);
                // deserialize_payload(payload);
                op_code_reception result_delete_segment = delete_memory_segment( 0 ); //ACTUALMENTE PASO UN ID = 0, PERO LUEGO NECESITO SABERLO DADO EL
                paquete = create_package( result_delete_segment ); 
                socket_send_package(paquete, cliente_fd);

                break;
            default:
                printf("\nError al recibir codigo de operacion \n");
                return (-1);
        }
    }
}