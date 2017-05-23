/*
    C ECHO client example using sockets
*/
#include<stdio.h> //printf
#include <stdlib.h>
#include<string.h>    //strlen
#include<sys/socket.h>    //socket
#include<arpa/inet.h> //inet_addr
#include<pthread.h> //for threading , link with lpthread

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

void *thr_send(void *par);
void mount_probe(TYPE_SONDA_MSG *msg,char *service);
TYPE_SONDA_MSG msg;

int main(int argc , char *argv[])
{
    int sock,size,*p;
    struct sockaddr_in server;
    char message[MAX_MESSAGE] , server_reply[MAX_MESSAGE];
    pthread_t send_thread;
	
    //Create socket
    sock = socket(AF_INET , SOCK_STREAM , 0);
    if (sock == -1)
    {
        printf("Could not create socket");
    }
    printf("Socket created size struct[%d]\n",sizeof(TYPE_SONDA_MSG));
     
    server.sin_addr.s_addr = inet_addr("127.0.0.1");
    server.sin_family = AF_INET;
    server.sin_port = htons( 9887 );
 
    //Connect to remote server
    if (connect(sock , (struct sockaddr *)&server , sizeof(server)) < 0)
    {
        perror("connect failed. Error");
        return 1;
    }
	if( (size=recv(sock , server_reply , MAX_MESSAGE , 0)) < 0){
		puts("recv failed");
		return -1;
	}
	server_reply[size]='\0';
	printf("Server reply :");
	puts(server_reply);
     
    puts("Connected\n");
	p = &sock;
	if( pthread_create( &send_thread , NULL ,  thr_send , (void*) p) < 0){
		perror("could not create thread");
		return 1;
	}

    //keep communicating with server
    while(1){
        //Receive a reply from the server
        if( (size=recv(sock , server_reply , 2000 , 0)) < 0)
        {
            puts("recv failed");
            break;
        }
        server_reply[size]='\0';
        printf("\nServer reply :[%s]\n",server_reply);
    }
     
    close(sock);
    return 0;
}

void * thr_send(void *socket_desc)
{
	int sock = *(int*)socket_desc;
	char message[MAX_MESSAGE],*p;
    while(1){
        printf("Enter message : ");
        scanf("%s" , message);
        if( strcmp(message,"msg") == 0 ){
			mount_probe(&msg,NULL);
			p = (char *)&msg;
			printf("Sending size...[%04d]\n",sizeof(TYPE_SONDA_MSG));
			sprintf(message,"%04d",sizeof(TYPE_SONDA_MSG));
			if( send(sock , message , 4 , 0) < 0) {
				printf("Send failed\n");
				exit(-1);
			}
			sleep(1); //enable this if you want to test server reading in 2 times
			printf("Sending data...[%s]\n",p);
			if( send(sock , p , sizeof(TYPE_SONDA_MSG) , 0) < 0) {
				printf("Send failed\n");
				exit(-1);
			}
			continue;
		}		
        //Send some data
        printf("Sending data...[%s]\n",message);
        if( send(sock , message , strlen(message) , 0) < 0) {
            puts("Send failed");
            exit(-1);
        }
    }

}

