import socket
import select
import sys

host = 'localhost'
port = 53002

#Socket creation for connection with server (server socket in client side)
server = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
#Connection to server using host and port
server.connect((host,port))

while True:
        #stores a list of possible inputs
        sockets_list = [sys.stdin, server]
        #Used for cases of input or output streams, returning read, write and error sockets 
        #from socket list
        read_sockets, write_sockets, error_sockets = select.select(sockets_list,[],[])

        #goes through sockets, if its server it's an input stream if not it send the message
        #of the user
        for socks in read_sockets:
            if socks == server:
                message = socks.recv(50).decode('utf-8')
                print(message)
            else:
                message = sys.stdin.readline()
                server.send(message.encode())    
server.close()    

    
  