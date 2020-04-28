import sys
from socket import socket, AF_INET, SOCK_DGRAM

SERVER_IP   = '192.168.1.201'
PORT_NUMBER = 5000
SIZE = 1

message=sys.argv[1]
print ("Test client sending packets to IP {0}, via port {1}\n".format(SERVER_IP, PORT_NUMBER))

mySocket = socket( AF_INET, SOCK_DGRAM )
mySocket.connect((SERVER_IP,PORT_NUMBER))

mySocket.send(message.encode('utf-8'))

