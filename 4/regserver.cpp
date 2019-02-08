#include <stdio.h>  			//printf, scanf
#include <unistd.h>				//for read()			
#include <sys/socket.h>			//for socket functions
#include <stdlib.h>       		//EXIT_FAILURE, memset
#include <string.h>				//strlen()
#include <netinet/in.h>			//for struct sockaddr
#include <pthread.h>			//for multi threading
#include <stdbool.h>			//for boolean var

#include <map>
#include <string>

#define backlog 5
#define PORT 6178
#define BUFFSIZE 1024
#define MAX_CLIENTS 10

using namespace std;

map<string,string> ipaddr;
map<string,string> status;

void *handle_client(void* client_socket)
{
	int client = *(int*)client_socket;

	char buf[BUFFSIZE];
	char ip[BUFFSIZE];

	while(1)
	{
		read(client, ip, BUFFSIZE);
		read(client, buf, BUFFSIZE);

		ipaddr[ip]=buf;
		status[ip]="Ready";

		string buffer="";

		map<string,string>::iterator it;
		for(it=ipaddr.begin();it!=ipaddr.end();++it)
			buffer=buffer+it->first+" : "+status[it->first]+"\n";

		memset(buf, 0, sizeof(buf));
		strcpy(buf, buffer.c_str());
		write(client, buf, BUFFSIZE);

		memset(buf, 0, sizeof(buf));
		read(client, buf, BUFFSIZE);

		buffer=string(buf);

		printf("%s\n",buf);

		if(strcmp(buf,"q")==0){
			ipaddr.erase(ip);
			status.erase(ip);
			break;
		}

		if(ipaddr.find(buffer)==ipaddr.end())
			strcpy(buf,"invalid");
		else if(status[buffer]=="Ready"){
			strcpy(buf,ipaddr[buffer].c_str());
			status[buffer]="Busy";
		}
		else strcpy(buf,"busy");

		write(client, buf, BUFFSIZE);
	}

	close(client);
	return NULL;
}

int main()
{
	int server_socket,client_socket,ret;
	struct sockaddr_in server_addr;
	int addrlen = sizeof(server_addr);

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

	pthread_t handler[MAX_CLIENTS];
	int n_conn;
	for(n_conn=0;n_conn<MAX_CLIENTS;n_conn++)
	{
		printf("Waiting for new connections %d\n",n_conn);
		client_socket = accept(server_socket, 
							(struct sockaddr *)&server_addr,
									(socklen_t*)&addrlen);

		pthread_create(&handler[n_conn], NULL, 
									handle_client, 
									(void*)&client_socket);
	}

	for(int i=0;i<MAX_CLIENTS;i++)
		pthread_join(handler[i],NULL);

	close(server_socket);

	return 0;
}