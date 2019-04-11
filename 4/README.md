# Assignment 4

***Note: Sample Images in `Images/` directory***

## Requirements
bash (for `showip.sh`)
gcc 

## Important
For the main registration server that maintains the peer records to be truly global and running 24x7, I kept the file `regserver.cpp` file on amazon aws ec2 instance and its public ip has been used in the `p2pchat.c` file. 
At the time of replicating the result, the amazon service may/maynot work due to it being a temporary account. You may need to change the address to your own private ip on a different port for testing purposes or create your own online vm instance.

## Steps
The file `regserver.cpp` should be running on a public ip or any IP which can be visible/accessible by all the intended peers and should always be active.  
```
g++ regserver.cpp -o server
./server
```
If you wish to keep the server process running in the background even after disconnecting `ssh` from the ec2 instance, use `screen`

Now, distribute the `p2pchat.c` and `showip.sh` among the intended users.
```
gcc p2pchat.c -o chat -lpthread
./chat
```

1. Enter your visible name.
2. A list of available online users will be printed.
3. Connect to one of them by entering their name.
4. Type the message and press `ENTER`.
5. Press `q` and `ENTER` to quit.
6. Refreshed list will be shown again, select a new peer to chat with or press `q` again to exit.

You can chat with only one person at a time.

## Working
The server and client processes of every peer are running in the same file in independant threads.
1. `login()` and `p2pserver()` will be running at the beginning.
2. The connection with main registration server will be established in the `login()` func and your `name` against your `ip` will be saved in a table there.
3. List of active users will be printed from this table, along with yourself , so you can chat with yourself if you don't have any friends :(
4. If they are not busy, then you get their ip address from registration server, then go to `p2pclient()` func with this ip as an argument to initiate a connection. The other peer has to be on the same private network.
5. Your `p2pclient()` establishes a connection with the already up and running `p2pserver()` on each peer (other peer) and parallel chatting happens using multi threading as solved in Assignment1.
6. If you are actively talking with somebody, then in the table of registration server, you will be marked as `busy`. Just a simple logic in the code is used for adding this feature.

***This code was not properly working on IIT-SES Network. Run on your own internet for correct working***