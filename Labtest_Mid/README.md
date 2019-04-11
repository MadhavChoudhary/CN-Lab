## Distributed Sorting
The four servers are in implemented in the same computer (INADDR_ANY) in this code.  

## How to run
```
chmod +x build.sh
./build.sh
```

This will compile the required files and create the required number of random numbers (you will be asked of how many random numbers you want to generate and they will be stored in data.txt)

Note: This no has to be divisible by 4.    

Now open 4 windows for 4 servers, and one window for a client.  
In these 4 windows, run server with your arbitrary port number.

```
./server <port>
```

In the main window
```
./client
```
Enter the port numebers `<port1> <port2> <port3> <port4>` and number of random numbers `<number>` (The same input you gave in build.sh file) when prompted.  

You numbers will be divided to four equal parts and will be sent to servers (in this code, the number has to be divisible by 4 as I am not handling the otherwise case.)  

The sorted numbers will be merged and printed in the terminal which may also be configured to go to the data.txt file if users wish.  
