import socket
import select
import sys

host = 'localhost'
port = 53002

server = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
server.connect((host,port))

while True:
        sockets_list = [sys.stdin, server]
        
        read_sockets, write_sockets, error_sockets = select.select(sockets_list,[],[])

        for socks in read_sockets:
            if socks == server:
                message = socks.recv(50).decode('utf-8')
                print(message)
            else:
                message = sys.stdin.readline()
                server.send(message.encode())    
server.close()    

    
  