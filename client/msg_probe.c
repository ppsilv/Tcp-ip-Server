#include<stdio.h>
#include <stdlib.h>
#include<string.h>
#include <unistd.h>

#include "msg_probe.h"

/*AGENTES
001-poseidon
002-printer_server
003-luminary_board1
004-hp-envy
*/
/*ID TERMINAL
10000000 poseidon
*/
static int num_tran=0;
#define		my_echo	100
char * get_d_h_now (char option)
{
	static char buffer[33];
    time_t rawtime;
    struct tm *info;

    time( &rawtime );

    info = localtime( &rawtime );
        if (option == 0){ /*0 = data - 1 = hora*/
        strftime(buffer,32,"20%y%m%d", info);
        }else{
        strftime(buffer,32,"%H%M%S", info);
        }
        return buffer;
}

void mount_probe(TYPE_SONDA_MSG *msg,char *service)
{
	memcpy(msg->cod_msg,"0600",4);
	memcpy(msg->cod_agente,"001",3);
	memcpy(msg->data_trans,get_d_h_now(0),8);
	memcpy(msg->hora_trans,get_d_h_now(1),6);
	sprintf(msg->num_trans,"%06d",num_tran);
	sprintf(msg->cod_acao,"%03d",my_echo);
	memcpy(msg->id_term,"10000000",8);
	memcpy(msg->mod_entr,"00",2);
	memcpy(msg->num_doc,"0000000000000000000",19);
	memcpy(msg->data_venc,"0000",4);
	memcpy(msg->novo_dado,"0000000000000000",16);
	memcpy(msg->filler,"000000000000000",15);
}
