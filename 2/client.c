//General
#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

//Network
#include <netdb.h>
#include <sys/socket.h>

#define MAX_REQUEST_SIZE 1000
#define BUFFSIZE 1024

int ret; 			//return value for error handling

//First get server information
struct addrinfo *getServerInfo(char *server, char *port)
{
	struct addrinfo hints, *res;

	//setup hints;
	memset(&hints, 0, sizeof(hints));
	hints.ai_family = AF_INET;
	hints.ai_socktype = SOCK_STREAM;

	if((ret=getaddrinfo(server, port, &hints, &res)))
	{
		printf("Failed to get addr information of server\n");
		return NULL;
	}

	return res;
}

//establish connection with the server
int establishConnection(struct addrinfo *info)
{
	if(info==NULL)
		return -1;

	int sockfd;
	for(;info!=NULL;info=info->ai_next)
	{
	    if((sockfd = socket(info->ai_family,info->ai_socktype,info->ai_protocol)) < 0) 
	    {
	      printf("Failed to create socket, moving to next one\n");
	      continue;
	    }

	    if(connect(sockfd, info->ai_addr, info->ai_addrlen) < 0) 
	    {
	      close(sockfd);
	      printf("Failed to connect, moving to next one\n");
	      continue;
    	}

    	freeaddrinfo(info);
    	printf("Connection Successful\n");
    	return sockfd;
	}

	freeaddrinfo(info);
	printf("Connection Unsucessful\n");
	return -1;
}

//send GET request
void sendGET(int sockfd, char *path)
{
	char req[MAX_REQUEST_SIZE]={0};
	sprintf(req, "GET %s HTTP/1.1\r\n\r\n", path);
	send(sockfd, req, strlen(req), 0);
}

//Main function
int main(int argc, char **argv)
{
	int sockfd;
	char buff[BUFFSIZE];

	if(argc!=4)
	{
		printf("Usage: <hostname> <port> <path>\n");
		return 1;
	}

	sockfd = establishConnection(getServerInfo(argv[1], argv[2]));

	if(sockfd==-1)
	{
		printf("Failed to connect to %s:%s/%s\n",argv[1],argv[2],argv[3]);
		return 3;
	}

	sendGET(sockfd, argv[3]);

	char *filename = strrchr(argv[3], '/');
	filename++;
	FILE *fp = fopen(filename, "w");

	while(recv(sockfd, buff, BUFFSIZE, 0)>0)
	{
		printf("%s",buff);
		fprintf(fp, "%s", buff);
		memset(buff, 0, BUFFSIZE);
	}

	fclose(fp);
	close(sockfd);

	char *command_str[MAX_REQUEST_SIZE] = {0};
	sprintf(command_str, "lynx -dump %s", filename);
	command(command_str);
	
	return 0;

}