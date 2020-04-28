import sys
import os
from socket import socket,gethostbyname,AF_INET,SOCK_DGRAM
hostname = gethostbyname('0.0.0.0')
port = 5000
size = 1


s = socket(AF_INET,SOCK_DGRAM)

s.bind((hostname,port))
arpSet = False
os.system("arp -d 192.168.1.200")

while True:
	(data,addr) = s.recvfrom(size)
	if (data == b'1' and not arpSet):
		arpSet = True
		os.system("sudo arp -s 192.168.1.200 0800271f3075")
	elif (data == b'0' and arpSet):
		arpSet = False
		os.system("arp -d 192.168.1.200")
