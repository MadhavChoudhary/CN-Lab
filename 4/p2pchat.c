#include <stdio.h>  			//printf, scanf
#include <unistd.h>				//for read()			
#include <sys/socket.h>			//for socket functions
#include <stdlib.h>       		//EXIT_FAILURE, memset
#include <string.h>				//strlen()
#include <netinet/in.h>			//for struct sockaddr
#include <pthread.h>			//for multi threading
#include <stdbool.h>			//for boolean var
#include <netdb.h> 				//for hostent
#include <arpa/inet.h> 			//for inet_ntoa,inet_addr

#define PORT 8080
#define PORT_REG 6178
#define backlog 5
#define BUFFSIZE 1024
#define REGSERVER "54.87.133.48"
#define clear() printf("\033[H\033[J")

int ret;						//for error handling
bool server;					//server connection status
bool cl;						//client connection status

void *read_client(void* client_socket)
{
	int client = *(int*)client_socket;
	char buffer[BUFFSIZE];

	while(cl)
	{
		memset(&buffer, 0, sizeof(buffer));
		read(client, buffer, sizeof(buffer));
		if(strcmp(buffer, "q")==0) cl=false;
		else printf("Message: %s\n",buffer);
	}

	return NULL;
}

void *write_client(void* client_socket)
{
	int client = *(int*)client_socket;
	char buffer[BUFFSIZE];

	while(cl)
	{
		memset(&buffer, 0, sizeof(buffer));
		fgets(buffer,sizeof(buffer),stdin);
		buffer[strlen(buffer)-1]='\0';
		write(client, buffer, sizeof(buffer));
		if(strcmp(buffer, "q")==0) cl=false;
	}

	return NULL;
}

void *read_server(void* client_socket)
{
	int client = *(int*)client_socket;
	char buffer[BUFFSIZE];

	while(server)
	{
		memset(&buffer, 0, sizeof(buffer));
		read(client, buffer, sizeof(buffer));
		if(strcmp(buffer, "q")==0) server=false;
		else printf("Message: %s\n",buffer);
	}

	return NULL;
}

void *write_server(void* client_socket)
{
	int client = *(int*)client_socket;
	char buffer[BUFFSIZE];

	while(server)
	{
		memset(&buffer, 0, sizeof(buffer));
		fgets(buffer,sizeof(buffer),stdin);
		buffer[strlen(buffer)-1]='\0';
		write(client, buffer, sizeof(buffer));
		if(strcmp(buffer, "q")==0) server=false;
	}

	return NULL;
}

void p2pclient(char *ip)
{
	printf("Came to p2pclient %s\n",ip);
	int client_socket;
	struct sockaddr_in server_addr;
	int addrlen = sizeof(server_addr);

	client_socket = socket(AF_INET, SOCK_STREAM, 0);
	if(client_socket == 0)
	{
		perror("socker failed\n");
		exit(EXIT_FAILURE);
	}
	server_addr.sin_addr.s_addr = inet_addr(ip);
	server_addr.sin_port = htons(PORT);
	server_addr.sin_family = AF_INET;

	ret = connect(client_socket, 
					(struct sockaddr *)&server_addr, 
						sizeof(server_addr));
	if(ret<0)
	{
		perror("Connection error");
		exit(EXIT_FAILURE);
	}

	cl=true;
	pthread_t read, write;

	pthread_create(&read, NULL, read_client, (void*)&client_socket);
	pthread_create(&write, NULL, write_client, (void*)&client_socket);

	pthread_join(read, NULL);
	pthread_join(write, NULL);

	close(client_socket);	
}

void *p2pserver()
{
	int server_socket,client_socket,ret;
	struct sockaddr_in server_addr;
	int addrlen = sizeof(server_addr);
	pthread_t read,write;

	server_socket = socket(AF_INET, SOCK_STREAM, 0);
	if(server_socket == 0)
	{
		perror("socker failed\n");
		exit(EXIT_FAILURE);
	}

	server_addr.sin_addr.s_addr = INADDR_ANY;
	server_addr.sin_port = htons(PORT);
	server_addr.sin_family = AF_INET;

	ret = bind(server_socket, (struct sockaddr *)&server_addr, 
								sizeof(server_addr));
	if(ret<0)
	{
		perror("bind failed");
		exit(EXIT_FAILURE);
	}

	ret = listen(server_socket, backlog);
	if(ret<0)
	{
		perror("listen failed");
		exit(EXIT_FAILURE);
	}

	while(1)
	{
		client_socket = accept(server_socket, 
						(struct sockaddr *)&server_addr,
								(socklen_t*)&addrlen);

		server=true;
		pthread_create(&read, NULL, 
								read_server, 
								(void*)&client_socket);
		pthread_create(&write, NULL, 
								write_server, 
								(void*)&client_socket);

		pthread_join(read, NULL);
		pthread_join(write, NULL);
	}

	close(client_socket);
	close(server_socket);

	return  NULL;
}

void *login()
{
	int client_socket;
	struct sockaddr_in server_addr;
	int addrlen = sizeof(server_addr);
	char name[BUFFSIZE];
	char name_backup[BUFFSIZE];
	char ip[BUFFSIZE];

	client_socket = socket(AF_INET, SOCK_STREAM, 0);

	if(client_socket == 0)
	{
		perror("socker failed\n");
		exit(EXIT_FAILURE);
	}
	server_addr.sin_addr.s_addr = inet_addr(REGSERVER);
	server_addr.sin_port = htons(PORT_REG);
	server_addr.sin_family = AF_INET;

	ret = connect(client_socket, 
					(struct sockaddr *)&server_addr, 
						sizeof(server_addr));

	printf("Came to login page!\n");
	if(ret<0)
	{
		perror("Connection error");
		exit(EXIT_FAILURE);
	}

	printf("Enter your visible name: ");
	memset(&name, 0, sizeof(name));
	scanf("%s",name);
	strcpy(name_backup,name);

	system("chmod +x showip.sh & ./showip.sh");

	FILE *fp;
	fp=fopen("ip.txt", "r");
	fscanf(fp,"%s",ip);
	fclose(fp);

	while(1)
	{
		if(!server)
		{
			clear();
			write(client_socket, name, BUFFSIZE);

			memset(&name, 0, sizeof(name));
			strcpy(name, ip);
			write(client_socket, name, BUFFSIZE);

			memset(&name, 0, sizeof(name));
			read(client_socket, name, BUFFSIZE);

			printf("%s\nConnect with?: ", name);
			memset(&name, 0, sizeof(name));
			scanf("%s", name);

			write(client_socket, name, BUFFSIZE);

			if(strcmp(name,"q")==0)
				break;
						
			memset(&name, 0, sizeof(name));
			read(client_socket, name, BUFFSIZE);

			if(strcmp(name,"busy") && strcmp(name,"invalid") && strlen(name)!=0)
				p2pclient(name);

			strcpy(name, name_backup);
		}
	}

	close(client_socket);

	return NULL;
}


int main()
{
	//attempt to login
	char ip[BUFFSIZE];
	char name[BUFFSIZE];
	server=false;
	cl=false;

	pthread_t regserver, p2pserv;

	pthread_create(&regserver, NULL, login, NULL);
	pthread_create(&p2pserv, NULL, p2pserver, NULL);

	pthread_join(regserver, NULL);
	//pthread_join(p2pserv, NULL);

	return 0;
}