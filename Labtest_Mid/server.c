#include <stdio.h>  			//printf, scanf
#include <unistd.h>				//for read()			
#include <sys/socket.h>			//for socket functions
#include <stdlib.h>       		//EXIT_FAILURE, memset
#include <string.h>				//strlen()
#include <netinet/in.h>			//for struct sockaddr
#include <pthread.h>			//for multi threading
#include <stdbool.h>			//for boolean var

#define NUMCONN 5
#define BUFFSIZE 1024
#define MAX_CLIENTS 5

int ret;

/* This function takes last element as pivot, places 
   the pivot element at its correct position in sorted 
    array, and places all smaller (smaller than pivot) 
   to left of pivot and all greater elements to right 
   of pivot */
int partition (int arr[], int low, int high) 
{ 
    int pivot = arr[high];    // pivot 
    int i = (low - 1);  // Index of smaller element 
  
    for (int j = low; j <= high- 1; j++) 
    { 
        // If current element is smaller than or 
        // equal to pivot 
        if (arr[j] <= pivot) 
        { 
            i++;    // increment index of smaller element 
            int temp=arr[j];
            arr[j]=arr[i];
            arr[i]=temp;
        } 
    } 
    // swap(&arr[i + 1], &arr[high]); 
    int temp=arr[i + 1];
    arr[i + 1]=arr[high];
    arr[high]=temp;

    return (i + 1); 
} 
  
/* The main function that implements QuickSort 
 arr[] --> Array to be sorted, 
  low  --> Starting index, 
  high  --> Ending index */
void quickSort(int arr[], int low, int high) 
{ 
    if (low < high) 
    { 
        /* pi is partitioning index, arr[p] is now 
           at right place */
        int pi = partition(arr, low, high); 
  
        // Separately sort elements before 
        // partition and after partition 
        quickSort(arr, low, pi - 1); 
        quickSort(arr, pi + 1, high); 
    } 
}

int main(int argc, char const *argv[])
{

	if(argc<2) return 0;
	int PORT = atoi(argv[1]);

	int server_socket,client_socket,ret;
	struct sockaddr_in server_addr;
	int addrlen = sizeof(server_addr);
	pthread_t clients[MAX_CLIENTS];

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

	ret = listen(server_socket, NUMCONN);
	if(ret<0)
	{
		perror("listen failed");
		exit(EXIT_FAILURE);
	}

	client_socket = accept(server_socket, 
							(struct sockaddr *)&server_addr,
									(socklen_t*)&addrlen);

	//receive and send

	int *number = malloc(sizeof(int));
	int i;
	read(client_socket, number, sizeof(number));
	int n=*number;
	int local_data[n];

	for(i=0;i<n;i++){
		read(client_socket, number, sizeof(number));
		local_data[i]=*number;
	}

	printf("Got %d Numbers\n",n);
	for(i=0;i<n;i++)
		printf("%d ",local_data[i]);
	printf("\n");

	quickSort(local_data, 0, n-1);

	printf("Sorted %d Numbers\n",n);
	for(i=0;i<n;i++)
		printf("%d ",local_data[i]);
	printf("\n");

	for(i=0;i<n;i++){
		*number = local_data[i];
		write(client_socket, number, sizeof(number));
	}	

	printf("Sent back Sorted Numbers\n");

	close(client_socket);
}