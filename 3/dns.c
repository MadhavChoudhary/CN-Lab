#include<stdio.h> 
#include<string.h>    
#include<stdlib.h>    
#include<sys/socket.h>    
#include<arpa/inet.h> 
#include<netinet/in.h>
#include<unistd.h>    

char dns_server[100];
char alt_dns_server[100];

#define T_A 1

void mad_get_hostbyname (unsigned char* , int);
void change_name_format (unsigned char*,unsigned char*);
unsigned char* mad_read (unsigned char*,unsigned char*,int*);

struct DNS_HEADER
{
    unsigned short id;
    unsigned char rd :1;
    unsigned char tc :1;
    unsigned char aa :1;
    unsigned char opcode :4;
    unsigned char qr :1;
    unsigned char rcode :4;
    unsigned char cd :1;
    unsigned char ad :1;
    unsigned char z :1;
    unsigned char ra :1;
    unsigned short q_count;
    unsigned short ans_count;
    unsigned short auth_count;
    unsigned short add_count;
};

struct QUESTION
{
    unsigned short qtype;
    unsigned short qclass;
};

#pragma pack(push, 1)
struct R_DATA
{
    unsigned short type;
    unsigned short _class;
    unsigned int ttl;
    unsigned short data_len;
};
#pragma pack(pop)

struct RES_RECORD
{
    unsigned char *name;
    struct R_DATA *resource;
    unsigned char *rdata;
};

typedef struct
{
    unsigned char *name;
    struct QUESTION *ques;
} QUERY;

int main( int argc , char *argv[])
{
    unsigned char hostname[100];

    strcpy(dns_server , "1.1.1.1");
    //strcpy(alt_dns_server, "8.8.4.4");

    printf("Hostname : ");
    scanf("%s" , hostname);

    mad_get_hostbyname(hostname , T_A);
    return 0;
}

void mad_get_hostbyname(unsigned char *host , int query_type)
{
    unsigned char buf[65536],*qname,*reader;
    int i , j , stop , s;
    struct sockaddr_in a;
    struct RES_RECORD answers[20],auth[20],addit[20];
    struct sockaddr_in dest;
    struct DNS_HEADER *dns = NULL;
    struct QUESTION *qinfo = NULL;
    printf("Resolving %s" , host);
    s = socket(AF_INET , SOCK_DGRAM , IPPROTO_UDP);
    dest.sin_family = AF_INET;
    dest.sin_port = htons(53);
    dest.sin_addr.s_addr = inet_addr(dns_server);

    dns = (struct DNS_HEADER *)&buf;
    dns->id = (unsigned short) htons(getpid());
    dns->qr = 0;
    dns->opcode = 0;
    dns->aa = 0;
    dns->tc = 0;
    dns->rd = 1;       //want recursive 
    dns->ra = 0;       
    dns->z = 0;
    dns->ad = 0;
    dns->cd = 0;
    dns->rcode = 0;
    dns->q_count = htons(1);   //only one question
    dns->ans_count = 0;
    dns->auth_count = 0;
    dns->add_count = 0;

    qname =(unsigned char*)&buf[sizeof(struct DNS_HEADER)];
    change_name_format(qname , host);
    qinfo =(struct QUESTION*)&buf[sizeof(struct DNS_HEADER) + (strlen((const char*)qname) + 1)];
    qinfo->qtype = htons( query_type );
    qinfo->qclass = htons(1);
    //printf("\nSending Packet...\n");
    if(sendto(s,(char*)buf,sizeof(struct DNS_HEADER) + (strlen((const char*)qname)+1) + sizeof(struct QUESTION),0,(struct sockaddr*)&dest,sizeof(dest)) < 0)
    {
        perror("sendto failed");
    }
    //printf("Done");

    i = sizeof dest;
    //printf("\nReceiving answer...");
    if(recvfrom (s,(char*)buf , 65536 , 0 , (struct sockaddr*)&dest , (socklen_t*)&i ) < 0)
    {
        perror("recvfrom failed");
    }
    //printf("Done");
    dns = (struct DNS_HEADER*) buf;

    reader = &buf[sizeof(struct DNS_HEADER) + (strlen((const char*)qname)+1) + sizeof(struct QUESTION)];

    stop=0;
    for(i=0;i<ntohs(dns->ans_count);i++)
    {
        answers[i].name=mad_read(reader,buf,&stop);
        reader = reader + stop;
        answers[i].resource = (struct R_DATA*)(reader);
        reader = reader + sizeof(struct R_DATA);
        if(ntohs(answers[i].resource->type) == 1) 
        {
            answers[i].rdata = (unsigned char*)malloc(ntohs(answers[i].resource->data_len));
            for(j=0 ;j<ntohs(answers[i].resource->data_len) ;j++)
            {
                answers[i].rdata[j]=reader[j];
            }
            answers[i].rdata[ntohs(answers[i].resource->data_len)] = '\0';
            reader = reader + ntohs(answers[i].resource->data_len);
        }
        else
        {
            answers[i].rdata = mad_read(reader,buf,&stop);
            reader = reader + stop;
        }
    }

    printf("\nAnswer Records : %d \n" , ntohs(dns->ans_count) );
    for(i=0 ;i < ntohs(dns->ans_count) ;i++)
    {
        printf("Name : %s ",answers[i].name);
        if( ntohs(answers[i].resource->type) == T_A) 
        {
            long *p;
            p=(long*)answers[i].rdata;
            a.sin_addr.s_addr=(*p);
            printf("has IPv4 address : %s",inet_ntoa(a.sin_addr));
        }
        if(ntohs(answers[i].resource->type)==5)
        {

            printf("has alias name : %s",answers[i].rdata);
        }
        printf("\n");
    }

    return;
}

u_char* mad_read(unsigned char* reader,unsigned char* buffer,int* count)
{
    unsigned char *name;
    unsigned int p=0,jumped=0,offset;
    int i , j;
    *count = 1;
    name = (unsigned char*)malloc(256);
    name[0]='\0';

    while(*reader!=0)
    {
        if(*reader>=192)
        {
            offset = (*reader)*256 + *(reader+1) - 49152;
            reader = buffer + offset - 1;
            jumped = 1;
        }
        else
        {
            name[p++]=*reader;
        }
        reader = reader+1;
        if(jumped==0)
        {
            *count = *count + 1;
        }
    }
    name[p]='\0';
    if(jumped==1)
    {
        *count = *count + 1;
    }

    for(i=0;i<(int)strlen((const char*)name);i++)
    {
        p=name[i];
        for(j=0;j<(int)p;j++)
        {
            name[i]=name[i+1];
            i=i+1;
        }
        name[i]='.';
    }
    name[i-1]='\0';
    return name;
}

void change_name_format(unsigned char* dns,unsigned char* host)
{
    int lock = 0 , i;
    strcat((char*)host,".");
    for(i = 0 ;i < strlen((char*)host) ;i++)
    {
        if(host[i]=='.')
        {
            *dns++ = i-lock;
            for(;lock<i;lock++)
            {
                *dns++=host[lock];
            }
            lock++;
        }
    }
    *dns++='\0';
}