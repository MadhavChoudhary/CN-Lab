# Assignment 3

***Note: Sample Images in `Images/` directory***

A DNS server query implementation in C using udp sockets

## Steps
```
gcc dns.c -o dns
./dns 
```
Getting ip address of a url/host by using `gethostbyname()` has already been implemented in Assignment2. In this program, a new `gethostbyname()` called `mad_get_host_by_name()` is being used to replace functionality of the default function.  

### Working
1. Using CloudFare as the dns server (IP=`1.1.1.1`). Another  dns server could be `8.8.8.8`(Google). The closest local dns servers being used by the system can be found by inspecting the file `/etc/resolve.conf`. A screen shot has been attached.
2. Class A records are being queried by sending udp packets 53 of dns servers in the standard dns packet format.  
3. You can shift between `iterative` to `recursive` dns calls by changing the `rd` field in DNS_Header from `0` to `1`
4. The program uses the format of DNS packet to construct the packet. By default the `rd` is set to 1, to force recursive dns call and `q_count` is set to `1` as our query is only to resolve one hostname. This can be changed based on requirement.
5. The return data is stored in another structure `RES_RECORD` meant for result dns records. Depending upon the `ans_count`, the answers will be printed.
