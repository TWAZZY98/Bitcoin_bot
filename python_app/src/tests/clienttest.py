import socket
import time

HEADER = 64
PORT = 5050
FORMAT = 'utf-8'
DISCONNECT_MESSAGE = "DISCONNECT"
SERVER = "172.20.10.6"
ADDR = (SERVER,PORT)
connected = False

client = socket.socket(socket.AF_INET, socket.SOCK_STREAM)

while not connected:
    print("[WARNING] Client test is trying to connect")
    try:
        client.connect(ADDR)
        connected = True
    except Exception as e:
        print(f"[ERROR] Unable to connect to the server")
        time.sleep(2)
            
def send(msg):
    message = msg.encode(FORMAT)
    msg_length = len(message)
    send_len = str(msg_length).encode(FORMAT)
    send_len += b' ' * (HEADER- len(send_len))
    client.send(message)

send("R")
while connected:
    try:
        price = client.recv(1024).decode(FORMAT)
        
        if not price:
            send(DISCONNECT_MESSAGE)
            connected = False
            break
        
        print(price)
        
    except Exception as e:
        print(f"[ERROR] could not recieve the message {type(e).__name__} {e}")
        connected = False