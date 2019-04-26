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

int ret;	//return value for error handling

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


    	//printf("Connection Successful\n");
    	return sockfd;
	}


	printf("Connection Unsucessful\n");
	return -1;
}

//send GET request
void sendGET(int sockfd, char *path)
{
	char req[MAX_REQUEST_SIZE]={0};
	sprintf(req, "GET %s HTTP/1.0\r\n\r\n", path);
	//printf("request: %s", req);
	send(sockfd, req, strlen(req), 0);
}

void getsrc(char *filename, char *addr, char *port, char *path)
{
    char command[BUFFSIZE] = {0};
    char *line = NULL;
    size_t len=0;
    ssize_t read;
    FILE *fp = fopen(filename, "r");

    while((read = getline(&line, &len, fp)) != -1)
    {
        if(strstr(line,"src="))
        {
            line = strtok(line,"\"");
            line = strtok(NULL, "\"");
            if(strrchr(filename,'/'))
            {
            	sprintf(strrchr(filename,'/')+1,"%s",line);
            	sprintf(command,"./client %s %s /%s 2",addr,port,filename);
            }
            
            sprintf(command,"./client %s %s %s 2",addr,port,line);
            system(command);
            // printf("%s\n",command);
        }
    }

    fclose(fp);
}

void show(int display, char *filename, char *addr, char *port, char *path){
	char command[BUFFSIZE] = {0};
	if(strstr(filename,"html"))
	{
		sprintf(command, "w3m -dump %s", filename);
		if(display>1) system(command);
		if(display>0)getsrc(filename,addr,port,path);
	}
	else if(strstr(filename,"jp"))
	{
		sprintf(command, "imgcat %s", filename);
		if(display>1) system(command);	
	}
}

//Main function
int main(int argc, char **argv)
{
	int sockfd;

	if(argc!=5)
	{
		printf("Usage: <hostname> <port> <path> <mode>\n");
		return 1;
	}

	int display = atoi(argv[4]);

	if( access(argv[3], F_OK ) != -1 ) {
	    printf("File already exists. Not Requesting.\n");
	    //show(display,argv[3],argv[1],argv[2],argv[3]);
	    return 0;
	} else {
	    printf("File doesnot exist. Requesting the webserver.\n");
	}

	sockfd = establishConnection(getServerInfo(argv[1], argv[2]));

	if(sockfd==-1)
	{
		printf("Failed to connect to %s:%s/%s\n",argv[1],argv[2],argv[3]);
		return 3;
	}

	// if(strcmp(argv[4],"GET")==0)
	// 	sendGET(sockfd, argv[3]);
	// else if(strcmp(argv[4],"POST")==0)
	// 	sendPOST(sockfd, argv[3]);

	sendGET(sockfd, argv[3]);
	
	int ptr=0;
	char buf;
	char buff[BUFFSIZE];

	//receive headers
	while(recv(sockfd, &buf, 1, 0) > 0){

	    buff[ptr++]=buf;
	    buff[ptr]='\0';

		if(strstr(buff, "\r\n\r\n")) break;

		printf("%c", buf);
	}

	char *filename = argv[3];

	FILE *fp = fopen(filename, "w");

	//receive the body
	while(recv(sockfd, &buf, 1, 0) > 0){
		fprintf(fp, "%c", buf);
		//printf("%c", buf);
	}

	fclose(fp);
	//close(sockfd);

	//show(display,filename,argv[1],argv[2],argv[3]);

	return 0;
}