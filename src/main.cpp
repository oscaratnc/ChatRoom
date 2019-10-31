#include <iostream>
#include <sys/types.h>
#include <unistd.h>
#include <sys/socket.h>
#include <netdb.h>
#include <arpa/inet.h>
#include<string.h>
#include<string>
#include<sstream>

#define PORT 53000


using namespace std;

int main() {
    int max_client = 30, addrlen;
    int client_socket[max_client];
    int max_sd, sd, new_socket, valread;

    char buffer[1025]; //data buffer of 1k

    //a message
    char *message ="ECHO Welcome to the Chat Room \n";

     //creates a set of socket descriptors
    fd_set master;

    //initialize all sockets to 0
    for (int i = 0; i < max_client; i++)
    {
        client_socket[i] = 0;
    }
    
    //create a socket
    int master_socket = socket(AF_INET, SOCK_STREAM, 0);
    if (master_socket == -1)
    {
        cerr << "Can't create a socket!";
        return -1;
    }
    
    //type of socket created
    sockaddr_in address;
    address.sin_family = AF_INET;
    address.sin_addr.s_addr = INADDR_ANY;
    address.sin_port = htons(PORT);

    // Bind the socket to a IP/Portn
    if( bind(master_socket, (sockaddr*)&address, sizeof(address)) == -1){
        cerr << "Can't bind to IP/port ";
        return -2;
    }
    printf("Listener on port %d \n", PORT);

    //Mark the socket for master_socket in 
    if(listen(master_socket, SOMAXCONN) == -1){
        cerr<< "Can't listen!";
        return -3;
    }

    //accept the incoming connection
    addrlen = sizeof(address);
    puts("Waiting for connections...");

    //running server that can habdle multiple connections:
    while (true)
    {
       
        // clears the socket  set
        FD_ZERO(&master);

        //add master_socket to the set
        FD_SET(master_socket, &master);
        max_sd = master_socket;

        //add child sockets to set
        for(int i =0; i< max_client; i++){

            //socket descriptor
            sd = client_socket[i];

            if(sd > 0){
                FD_SET(sd, &master);
            }

            if (sd >max_sd) {
                max_sd = sd;
            }
            
        }

        //select allows to monitor multiple file descriptors  waiting until one or more of the file descriptors
        //become "ready" for some class of I/O operation.
        int socketCount = select(max_sd +1 ,&master, nullptr, nullptr, nullptr);

        if ((socketCount <0) && (errno != EINTR) ){
            printf("select error");
        }

        //If something happened on the master socket,
        //then its an incoming connection
        if(FD_ISSET(master_socket, &master))
        {   
            if (( new_socket = accept(master_socket, 
                    (sockaddr *)&address, (socklen_t*)&addrlen)) < 0)
            {
                printf("Accept error");
                exit(EXIT_FAILURE);
            }

            //Inform user of socket number - used in send and recieve commands
            printf("New Connection, socket fd is %d, ip is: %s, port: %d, \n", 
                    new_socket, inet_ntoa(address.sin_addr), ntohs(address.sin_port));
            
            //Send new connection greeting message
            if (send(new_socket, message,strlen(message),0) != strlen(message))
            {
                printf("send");
            }

            puts("Welcome message sent succesfully");
                
            //add new socket to array of sockets
            for (int i = 0; i < max_client; i++)
            {
                //if position is empty
                if (client_socket[i] == 0)
                {
                    client_socket[i] = new_socket;
                    printf("Adding to list of sockets as %d \n",i);
                    break;
                }
                    
            }      
        }

        //else its some IO operation on some other socket
        for (int i = 0; i < max_client; i++)
        {
           sd = client_socket[i];
           if (FD_ISSET(sd , &master))
           {
               //Check if it was for closing, and also read the incoming message
               if((valread = read(sd, buffer, 1024)) == 0){
                    //Somebody disconnected, get hist details and print
                    getpeername(sd, (sockaddr*)&address, (socklen_t*)&addrlen);
                    printf("Host disconnected, ip %s, port %d \n ", inet_ntoa(address.sin_addr), ntohs(address.sin_port));

                    //Close the socker and mark as 0 in list for reuse
                    close(sd);
                    client_socket[i] == 0 ;
               }

               //Echo back the message that came in 
               else{
                   //set the string terminating NULL byte on the end of the data read.
                   buffer[valread] = '\0';

                   for(int i = 0; i< max_client; i++){
                       if(sd != client_socket[i] && master_socket != client_socket[i]){
                        
                        ostringstream ss;
                        ss << "Client #"<<sd<<": " <<buffer;
                        string strOut = ss.str();
                        send(client_socket[i], strOut.c_str(), strOut.size()+1, 0);
                           
                       }
                   }
               }
           }
           
        }
        

    
    
    }
    return 0;

}



    // //Accept a call
    // sockaddr_in client;
    // socklen_t clientSize = sizeof(client);
    // char host[NI_MAXHOST];
    // char service[NI_MAXSERV];

    // int clientSocket  = accept(master_socket, (sockaddr*)&client, &clientSize);

    // if (clientSocket == -1 ){
    //     cerr<< "Problem with client connecting!";
    //     return -4;
    // }

    // //Close the master_socket sockets
    // close(master_socket);
    // memset(host, 0, NI_MAXHOST);
    // memset(service, 0 , NI_MAXSERV);
    // int result = getnameinfo( (sockaddr*)&client, sizeof(client), 
    //                            host, NI_MAXHOST, service, NI_MAXSERV, 0);

    // if (result)
    // {   
    //     cout << host << "connected on " << service << endl;
    // }
    // else{
    //     inet_ntop(AF_INET, &client.sin_addr, host, NI_MAXHOST);
    //     cout<< host << "connected on " << ntohs(client.sin_port) << endl;
    // }


    // //While recieveing display message echo message.
    // char buf[4096];
    // while(true){
    //     //clear the buffer 
    //     memset(buf , 0, 4096);

    //     //&Wait for a message 
    //     int bytesRecv = recv(clientSocket, buf, 4096, 0);
    //     if (bytesRecv == -1)
    //     {
    //         cerr << "There was a connection issue" << endl;
    //         break;
    //     }
        
    //     if (bytesRecv == 0){
    //         cout << "the client disconnected" << endl;
    //         break;
    //     }
        
         
    //     // Display message 

    //     cout << "Recieved: " << string(buf, 0, bytesRecv) << endl;
    //     // Resend message

    //     send( clientSocket, buf, bytesRecv+1, 0);

    // }
    // // Close socket
   // close(clientSocket);