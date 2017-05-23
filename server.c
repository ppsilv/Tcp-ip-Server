#include<stdio.h>
#include<string.h>    
#include<stdlib.h>    
#include<sys/socket.h>
#include<arpa/inet.h> 
#include<unistd.h>    
#include<pthread.h>

typedef struct sonda_msg{
	char cod_msg[4];
	char cod_agente[3];
	char data_trans[8];
	char hora_trans[6];
	char num_trans[6];
	char cod_acao[3];
	char id_term[8];
	char mod_entr[2];
	char num_doc[19];
	char data_venc[4];
	char novo_dado[16];
	char filler[15];
} TYPE_SONDA_MSG;

#define MAX_MESSAGE 2000

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
    char *message , client_message[MAX_MESSAGE], msg[MAX_MESSAGE],tmp[4+1];
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
		read_size = recv(sock , client_message , 4 , 0);
		if ( read_size <= 0 ){
			close(sock);
			break;
		}
		if ( read_size < 4 ){
			printf("you need to send firstly 4 bytes of message size\n");
			continue;
		}
		memcpy(tmp,client_message,4);
		tmp[4]='\0';
		msg_size = atoi(tmp);
		//printf("Size of message[%d]\n",msg_size);
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
