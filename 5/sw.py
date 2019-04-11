#!/usr/bin/python
import socket
import threading
import random
import time
import sys

SERVER = ('127.0.0.1', 8080)
CLIENT = ('127.0.0.1', 8081)
n = int(sys.argv[1])

def make_packet(seq):
    data = bin(random.randint(0,5000))
    return str(seq)+','+data

def send_ack(sock, message):
    delay = random.random()*1.5
    time.sleep(delay)
    sock.sendto(message.split(',')[0], CLIENT)
    exit()

def server():
    sock = socket.socket(socket.AF_INET, socket.SOCK_DGRAM)
    sock.bind(SERVER)

    sock.settimeout(2)
    while True:
        try:
            (message, addr) = sock.recvfrom(1024)
        except:
            break
        p = random.randint(0,9)
        if(p<8):        #Simulating Delayed ACK
            threading.Thread(target=send_ack, args=(sock, message)).start()
        #else do not send ack


def client():
    sock = socket.socket(socket.AF_INET, socket.SOCK_DGRAM)
    sock.bind(CLIENT)

    ack = []
    messages = []
    for i in range(n):
        ack.append(0)
        messages.append(make_packet(i))
    
    sock.settimeout(0.5)  #waits for 0.5 sec

    i=0
    while i<n:
        cnt=0
        while ack[i]==0:
            #send message
            sock.sendto(messages[i], SERVER)

            #recieve message
            try:
                (data, addr) = sock.recvfrom(1024)
                ack[int(data)]=1
            except:
                continue
            cnt=cnt+1
            print (ack)
        print ("Packet %d Sent in %d attempts" % (i,cnt))
        i=i+1


if __name__ == '__main__':

    t1 = threading.Thread(target=server)
    t2 = threading.Thread(target=client)

    t1.start()
    t2.start()

    t1.join()
    t2.join()