# Assignment 5

***Note: Sample Images in `Images/` directory***

## Requirements

Python3 (preferable)  

## Steps
Run the file by
```
python sw.py <no_of_packets>
```

## Working
1. The sender and receiver will be running in the same program in different threads. Server will run at `8080` and receiver at `8081`. 
2. Server will create randomly generated `n` packets given by the user and send them one by one until the ack is received from the receiver.
3. Packet loss, Ack loss and delayed Ack are being considered for this simulation.