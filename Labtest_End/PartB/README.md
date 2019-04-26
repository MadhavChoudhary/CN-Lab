## LSR NS2 Simulation

***Note: Sample Images in `Images/` directory***

### Instructions
To generate `.tr` and `.nam` files again (fresh)
```
ns lsProg.tcl
```
To use existing `.nam` file (for simulation)
```
ns 
nam thro.nam
```
To use the existing `.tr` file (for faster analysis)
```
python analysis.py
```

### Working
1. lsr protocol has been implemented by using rtProto LS, a defined protocol in ns2.
2. The simulation has been done on a custom topology consisting of 11 nodes.
3. Link failure has also been simulated as seen in the tcl file.
4. The analysis.py calculates the avg throughput of cbr packets.