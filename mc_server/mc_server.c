#include<stdio.h>
#include<string.h>    
#include<stdlib.h>    
#include<sys/socket.h>
#include<arpa/inet.h> 
#include<unistd.h>    
#include<pthread.h>

#include "msg_probe.h"

void *connection_handler(void *);
  
void sleep_ms(int milliseconds) // cross-platform sleep function
{
#ifdef WIN32
    Sleep(milliseconds);
#elif _POSIX_C_SOURCE >= 199309L
    struct timespec ts;
    ts.tv_sec = milliseconds / 1000;
    ts.tv_nsec = (milliseconds % 1000) * 1000000;
    nanosleep(&ts, NULL);
#else
    usleep(milliseconds * 1000);
#endif
}

int main(int argc , char *argv[])
{
    int socket_desc , client_sock , c , *new_sock;
    struct sockaddr_in server , client;
     
    socket_desc = socket(AF_INET , SOCK_STREAM , 0);
    if (socket_desc == -1)
    {
        printf("Could not create socket\n");
    }
    printf("Socket created\n");
     
    server.sin_family = AF_INET;
    server.sin_addr.s_addr = INADDR_ANY;
    server.sin_port = htons( 9887 );
     
    if( bind(socket_desc,(struct sockaddr *)&server , sizeof(server)) < 0){
        perror("bind failed. Error");
        return 1;
    }
    listen(socket_desc , 3);
    c = sizeof(struct sockaddr_in);
    printf("Waiting for incoming connections...\n");
    c = sizeof(struct sockaddr_in);
    while( (client_sock = accept(socket_desc, (struct sockaddr *)&client, (socklen_t*)&c)) )
    {
        printf("Connection accepted\n");
         
        pthread_t sniffer_thread;
        new_sock = malloc(1);
        *new_sock = client_sock;
         
        if( pthread_create( &sniffer_thread , NULL ,  connection_handler , (void*) new_sock) < 0)
        {
            perror("could not create thread");
            return 1;
        }         
    }
     
    if (client_sock < 0)
    {
        perror("accept failed");
        return 1;
    }
     
    return 0;
}
 
void *treatMessage(char *msg)
{
	TYPE_SONDA_MSG *tmp=(TYPE_SONDA_MSG *)msg;
	memcpy(tmp->cod_acao,"999",3);
}

void *connection_handler(void *socket_desc)
{
    //Get the socket descriptor
	int fltimeout=0;
    int sock = *(int*)socket_desc,msg_size,msg_lido=0;
    int read_size;
    char tmp1[5],*message , client_message[MAX_MESSAGE], msg[MAX_MESSAGE],tmp[4+1];
    int timeout=10;
	int time_out=0;
	message = "Greetings! I am your connection handler\n";
    write(sock , message , strlen(message));
          
    while( 1 ){
		printf("Aguardando message\n");
		memset(client_message,0,MAX_MESSAGE);
		memset(msg,0,MAX_MESSAGE);
		msg_lido=0;
		msg_size=0;
		//Lendo tamanho da mensagem
get_size:
		read_size = read(sock, client_message, 4);//recv(sock , client_message , 40 , 0);
		if ( read_size <= 0 ){
			close(sock);
			break;
		}
		printf("Lido [%d]\n",read_size);
		if ( read_size < 4 ){
			printf("you need to send firstly 4 bytes of message size\n");
			printf("you are sending less than 4 bytes, review you client send routine...\n");
			continue;
		}
		memcpy(tmp,client_message,4);
		tmp[4]='\0';
		msg_size = atoi(tmp);
		if(msg_size > sizeof(TYPE_SONDA_MSG)){
			printf("Size of message[%d]\n",msg_size);
			printf("Descartando mensagem\n");
			while( recv(sock , client_message , 10 , 0 ) >0 );
			printf("Descartado a mensagem\n");
		}
		while( 1 ){
			read_size = recv(sock , client_message , msg_size , MSG_DONTWAIT );
			if( read_size > 0){
				msg_lido += read_size;
				client_message[read_size]='\0';
				if ( ( msg_lido == msg_size) ){
					strcat(msg,client_message);
					message = msg;
					break;
				}
				strcat(msg,client_message);
			}
		} 
		if ( fltimeout ){
			fltimeout=0;
			
			printf("continuando size[%d]\n",read_size);
			continue;
		}
		treatMessage(message);
		write(sock , message , strlen(message));		
    }
     
	printf("Terminando atendimento ao cliente...\n");		
    if(read_size == 0)
    {
        printf("Client disconnected\n");
        fflush(stdout);
    }
    else if(read_size == -1)
    {
        perror("recv failed");
    }         
    free(socket_desc);
    return(0);
}

/*
void *connection_handler(void *socket_desc)
{
    //Get the socket descriptor
    int sock = *(int*)socket_desc,msg_size,msg_lido=0;
    int read_size;
    char *message , client_message[MAX_MESSAGE], msg[MAX_MESSAGE];
    int timeout=10;
	int time_out=0;
	message = "Greetings! I am your connection handler\n";
    write(sock , message , strlen(message));
          
    while( 1 ){
		printf("Aguardando message2\n");
		memset(client_message,0,MAX_MESSAGE);
		memset(msg,0,MAX_MESSAGE);
		read_size = recv(sock , client_message , MAX_MESSAGE , 0);
		if( read_size > 0){
			if( read_size < 4 ){
				//printf("descartando size da mensagem a ser lida...total de bytes do tamanho[%d]\n",read_size);
				continue;
			}
			printf("Size read...[%d]\n",read_size);
			client_message[read_size]='\0';
			if ( read_size > 4 ){
				printf("read_size > 4\n");
				char tmp[4+1];
				memcpy(tmp,client_message,4);
				tmp[4]='\0';
				msg_size = atoi(tmp);
				printf("msg_size [%d]\n",msg_size);
				if ( read_size >= msg_size ){
					message = client_message;
					message+=4;
				}
			}else{
				printf("read_size <= 4\n");
				msg_size = atoi(client_message);
				time_out=0;
				while( 1 ){
					read_size = recv(sock , client_message , MAX_MESSAGE , MSG_DONTWAIT );
					if( read_size > 0){
						msg_lido += read_size;
						client_message[read_size]='\0';
						printf("msg_lido[%d]>=msg_size[%d]\n",msg_lido,msg_size);
						if ( ( msg_lido == msg_size) ){
							strcat(msg,client_message);
							write(sock , msg , strlen(msg));
							msg_lido=0;
							msg_size=0;
							printf("Limpando...[%d][%d]\n",msg_lido,msg_size);
							break;
						}else  if ( ( msg_lido > msg_size ) ){
							printf("msg_lido > msg_size\n");
							msg_lido=0;
							msg_size=0;
							break;
						}
						strcat(msg,client_message);
						//write(sock , msg , strlen(msg));
					}
					time_out++;
					sleep(1);
					if ( time_out >= timeout ){
							printf("timeout de leitura saindo...\n");
							msg_lido=0;
							msg_size=0;
							break;
					}
					message = client_message;
				}
			}
		}else if( read_size <= 0 ){
			printf("Connection closed...\n");
			close(sock);
			return;
		}
		printf("Tratando mensagem\n");
		printf("Message[%s]\n",msg);
		printf("Message[%s]\n",message);
		//treatMessage(message);
		printf("Message[%s]\n",msg);
		printf("Message[%s]\n",message);
		write(sock , message , strlen(message));		
    }
     
	printf("Terminando atendimento ao cliente...\n");		
    if(read_size == 0)
    {
        printf("Client disconnected");
        fflush(stdout);
    }
    else if(read_size == -1)
    {
        perror("recv failed");
    }
         
    //Free the socket pointer
    free(socket_desc);
     
    return 0;
}
*/

