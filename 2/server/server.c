//General
#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

//Network
#include <netdb.h>
#include <sys/socket.h>

#define BUFFSIZE 1024

int ret; 	//Return value for error handling purposes

//Get addr information
struct addrinfo *getAddrInfo(char* port) 
{
    struct addrinfo hints, *res;

    //setup hints;
    memset(&hints, 0, sizeof(hints));
    hints.ai_family = AF_INET;
    hints.ai_flags = AI_PASSIVE;
    hints.ai_socktype = SOCK_STREAM;

    if((ret=getaddrinfo(NULL, port, &hints, &res)))
    {
        printf("Failed to get addr information of server\n");
        return NULL;
    }

    return res;
}

//Bind the server to the port
int bindListener(struct addrinfo *info) 
{
    if(info==NULL)
        return -1;

    int sockfd;
    for(;info!=NULL;info=info->ai_next)
    {
        if((sockfd = socket(info->ai_family,info->ai_socktype,info->ai_protocol)) < 0) 
        {
          printf("Failed to bind socket, moving to next one\n");
          continue;
        }

        int opt = 1;
        if(setsockopt(sockfd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(int)) < 0) 
        {
          printf("Failed to reuse socket, moving to next one\n");
          return -1;
        }
        if(bind(sockfd, info->ai_addr, info->ai_addrlen) < 0) 
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

//Send the response status line
void header(int handler, int status)
{
    char header[BUFFSIZE] = {0};

    if(status==0) sprintf(header, "HTTP/1.0 200 OK\r\n");
    else if(status==1) sprintf(header, "HTTP/1.0 403 Forbidden\r\n");
    else sprintf(header, "HTTP/1.0 404 Not Found\r\n");

    send(handler, header, strlen(header), 0);
}

//get mime types
char *getMimeType(char *type)
{
    char buff[BUFFSIZE] = {0};
    char *line = NULL;
    size_t len=0;
    ssize_t read;
    FILE *fp = fopen("mime_type.txt", "r");

    while((read = getline(&line, &len, fp)) != -1)
    {
        line = strtok(line, " ");
        if(strstr(line,type))
        {
            line = strtok(NULL,"\n");
            break;
        }
    }

    fclose(fp);

    return line;
}

//To send content length, host, date&time, type..
void sendContentHeaders(int handler, char *filename)
{
    char header[BUFFSIZE] = {0};

    sprintf(header, "Server: Mad Server\r\n");
    sprintf(header+strlen(header), "Connection: close\r\n");

    char *type = getMimeType(strrchr(filename, '.'));
    sprintf(header+strlen(header), "Content-Type: %s\r\n",type);

    FILE *fp;
    long int size=0;
    fp = fopen(filename, "r");
    fseek(fp, 0 ,SEEK_END);
    size = ftell(fp);

    sprintf(header+strlen(header), "Content-Length: %ld\r\n\r\n", size);
    sprintf(header+strlen(header), "Cookie: %ld\r\n\r\n", handler);

    printf("%s",header);

    send(handler, header, strlen(header), 0);
}


//Resolve the request
void resolve(int handler)
{
    char buff[BUFFSIZE] = {0};
    char *method;
    char *filename;

    //receive the request and get the method
    recv(handler, buff, BUFFSIZE, 0);
    method = strtok(buff, " ");
    if(strcmp(method, "GET") == 0){
        filename = strtok(NULL, " ");
        if (filename[0] == '/') filename++;
    }
    else if(strcmp(method, "POST")==0){
        filename="post.txt";
    }

    //send status code
    if(access(filename, F_OK)!=0)
    {
        header(handler, 2);
        return;
    }
    else if(access(filename, R_OK)!=0)
    {
        header(handler, 2);
        return;
    }
    else header(handler, 0);

    //generate content headers for the file
    sendContentHeaders(handler, filename);

    FILE *fp = fopen(filename, "r");
    char buf;
    while(!feof(fp))
    {
        buf = fgetc(fp);
        send(handler, &buf, 1, 0);
        //printf("%c",buf);
    }
}

int main(int argc, char **argv)
{
    if(argc!=2)
    {
        printf("Usage: ./server <port>\n");
        return 1;
    }

    int sockfd = bindListener(getAddrInfo(argv[1]));

    if(sockfd<0)
    {
        printf("Failed to bind to the port\n");
        return 2;
    }

    if(listen(sockfd, 20)<0)
    {
        printf("Error in listening\n");
        return 3;
    }

    int handler;
    socklen_t size;
    struct sockaddr_storage client;
    size = sizeof(client);

    while(1)
    {
        handler = accept(sockfd, (struct sockaddr *)&client, &size);

        if(handler<0)
        {
            printf("Failed to accept\n");
            continue;
        }
        resolve(handler);
        close(handler);
    }

    close(sockfd);
    return 0;
}