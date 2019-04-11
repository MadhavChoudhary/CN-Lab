#include <stdio.h>  			//printf, scanf
#include <unistd.h>				//for read()			
#include <sys/socket.h>			//for socket functions
#include <stdlib.h>       		//EXIT_FAILURE, memset
#include <string.h>				//strlen()
#include <netinet/in.h>			//for struct sockaddr
#include <pthread.h>			//for multi threading
#include <stdbool.h>			//for boolean var

#define BUFFSIZE 1024

int n;
int *data;

int port1=8005;
int port2=8006;
int port3=8007;
int port4=8008;

int ret;

void merge(int arr[], int l, int m, int r) 
{ 
    int i, j, k; 
    int n1 = m - l + 1; 
    int n2 =  r - m; 
  
    /* create temp arrays */
    int L[n1], R[n2]; 
  
    /* Copy data to temp arrays L[] and R[] */
    for (i = 0; i < n1; i++) 
        L[i] = arr[l + i]; 
    for (j = 0; j < n2; j++) 
        R[j] = arr[m + 1+ j]; 
  
    /* Merge the temp arrays back into arr[l..r]*/
    i = 0; // Initial index of first subarray 
    j = 0; // Initial index of second subarray 
    k = l; // Initial index of merged subarray 
    while (i < n1 && j < n2) 
    { 
        if (L[i] <= R[j]) 
        { 
            arr[k] = L[i]; 
            i++; 
        } 
        else
        { 
            arr[k] = R[j]; 
            j++; 
        } 
        k++; 
    } 
  
    /* Copy the remaining elements of L[], if there 
       are any */
    while (i < n1) 
    { 
        arr[k] = L[i]; 
        i++; 
        k++; 
    } 
  
    /* Copy the remaining elements of R[], if there 
       are any */
    while (j < n2) 
    { 
        arr[k] = R[j]; 
        j++; 
        k++; 
    } 
}

void *server(void *q)
{
	// printf("Came Here\n");
	int port = *(int*)q;

	int local_data[n/4];
	int i,start;

	if(port==port1)
		start=0;

	else if(port==port2)
		start=n/4;

	else if(port==port3)
		start=n/2;

	else if(port==port4)
		start=(3*n)/4;

	for(i=start;i<start+(n/4);i++)
		local_data[i-start]=data[i];

	// for(i=0;i<n/4;i++)
	// 	printf("%d ", local_data[i]);

	int client_socket;
	struct sockaddr_in server_addr;
	int addrlen = sizeof(server_addr);

	char buffer[BUFFSIZE];

	client_socket = socket(AF_INET, SOCK_STREAM, 0);

	if(client_socket == 0)
	{
		perror("socker failed\n");
		exit(EXIT_FAILURE);
	}

	server_addr.sin_addr.s_addr = INADDR_ANY;
	server_addr.sin_port = htons(port);
	server_addr.sin_family = AF_INET;

	ret = connect(client_socket, 
					(struct sockaddr *)&server_addr, 
						sizeof(server_addr));

	if(ret<0)
	{
		perror("Connection error");
		exit(EXIT_FAILURE);
	}

	int *number = malloc(sizeof(int));

	*number = n/4;
	write(client_socket, number, sizeof(number));

	for(i=0;i<n/4;i++){
		*number = local_data[i];
		write(client_socket, number, sizeof(number));
	}

	for(i=0;i<n/4;i++){
		read(client_socket, number, sizeof(number));
		local_data[i]=*number;
		// printf("%d\n",local_data[i]);
	}	

	for(i=start;i<start+(n/4);i++)
		data[i]=local_data[i-start];

	close(client_socket);

	return NULL;
}

int main()
{
	//attempt to login
	pthread_t conn1,conn2,conn3,conn4;

	int count=0,i;
	printf("Enter the port numbers\n");
	scanf("%d %d %d %d", &port1, &port2, &port3, &port4);

	printf("Enter the number of numbers:\n");
	scanf("%d", &n);
	data = malloc(n*sizeof(int));

	FILE *fp;
    char *line = NULL;
    size_t len=0;
    ssize_t read;
	fp = fopen("data.txt", "r");

    while((read = getline(&line, &len, fp)) != -1)
    {
    	line[strlen(line)-1]='\0';
    	data[count++]=atoi(line);
    }

	pthread_create(&conn1, NULL, server, (void*)&port1);
	pthread_create(&conn2, NULL, server, (void*)&port2);
	pthread_create(&conn3, NULL, server, (void*)&port3);
	pthread_create(&conn4, NULL, server, (void*)&port4);

	pthread_join(conn1, NULL);
	pthread_join(conn2, NULL);
	pthread_join(conn3, NULL);
	pthread_join(conn4, NULL);

	printf("Got:");

	for(i=0;i<n;i++){
		if(i%(n/4)==0) printf("\n");
		printf("%d ",data[i]);
	}
	printf("\n");	

	merge(data, 0, n/4-1, n/2-1);
	merge(data, n/2, (3*n)/4-1, n-1);
	merge(data, 0, n/2-1, n-1);

	printf("Merged:\n");

	for(i=0;i<n;i++)
		printf("%d ",data[i]);
	printf("\n");

	return 0;
}