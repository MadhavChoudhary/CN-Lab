# Assignment 1

A Multi-Threaded Chat/File Transfer tool from one computer to others on a common Network.

## Requirements:

Ubuntu / other Linux-based distros
MacOS

## Steps:
The client will run at `../` folder and server will run in `./` folder by default 
```
	chmod +x build.sh
	chmod +x clean.sh
	./build.sh
```
## Options:

To run chatbox code, in one terminal at `./` ,and other terminal at `../`
```
	./server
	./client
```
each in different terminal windows.

To clean the executables formed
```
	./clean.sh
``` 

## Testing:

The PORT used is `8080` by default. It can be changed by changing the macro in both server.c and client.c. Run server and client from different directories on same computer or between different computers to test the file transfer part. The code has ip address set to `INADDR_ANY`. So it works within a computer. To communicate between different computers, use the ip address of that computer in that line. 

## Note:

Make sure you give the same port no input for server and client.
The list of files availiable for download will be sent by the server. The client has to select the file among that list. The selected file will be downloaded and a progress bar will indicate the status of download.