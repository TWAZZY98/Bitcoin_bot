#this server will send data from data queue to port to the cpp app
import socket
from dataqueue import databuffer as db
import threading
import time

HEADER = 64
PORT = 5050
SERVER = socket.gethostbyname(socket.gethostname())
ADDR = (SERVER,PORT)
FORMAT = 'utf-8'
DISCONNECT_MESSAGE = "DISCONNECT"

#"""
#  add another thread that checks commands to accomodate disconnecting and reconnecting 
# 
# """

server = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
server.bind(ADDR)

def start_transmission(conn):
    previous_price = None
    
    while True:
        current = db.latest_price
        if current != previous_price:
            try:
                conn.send(str(current).encode(FORMAT))
            except Exception as e:
                print(f"[ERROR] server could not send a packet {type(e).__name__} {e}")
            previous_price = current
        time.sleep(0.1)

def handle_client(conn,addr):
    print(f"[INFO] new connection at: {addr}")
    
    
    while True:
        msg = conn.recv(HEADER).decode(FORMAT)

        if msg == DISCONNECT_MESSAGE:
            print("[INFO] Client disconnected")
            break
        if msg == "R":
            start_transmission(conn)
            
    conn.close()
                
            
                
            

def start():
    print("[INFO] server is starting")
    server.listen()
    while True:
        conn, addr = server.accept()
        thread = threading.Thread(target=handle_client, args=(conn,addr))
        thread.start()
        print(f"[INFO] number of active connections: {threading.activeCount() -3}")
        