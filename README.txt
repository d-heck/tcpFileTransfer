@ Author: David Heck (dheck@udel.edu)

This program is a TCP file transfer socket programming example written in C.  
All code has been tested in an Ubuntu Linux environment using the gcc compiler.

Files + Folders:                Desription:
    /client/client.c                Source C code for client socket.
    /server/server.c                Source C code for server socket.
    /server/long_test.txt           A long text file for transferring (on server)
    /server/test.jpg                A .jpg file for transferring (on server)
    /server/test.png                A .png file for transferring (on server)
    /server/testA.txt               A short text file for transferring (on server)
    /server/testB.txt               A short text file for transferring (on server)


Compile Instructions:
    gcc server.c -o server 
    gcc client.c -o client 


To Run:
    ./server [listening port number]
    ./client [destination ip] [destination port number]

    Be sure to run the server file before the client, as it will need to be listening on the specified port beforehand.
    Both programs can be run simply by "./exec" no arguments are necessary; however, arguments can be passed through.

    Server Note: if no listening port ardument is passed through, it defaults to 8080.
    Client Note: if no arguments are passed it defaults to 127.0.0.1 on port 8080. If no port is specified it defaults to 8080.


When Running:
    The client will connect to the running server via TCP connection. After connecting, the client side will prompt for a file name.
    The file name can be any file that is within the /server/ directory. Once transferred it will appear in the /client/ directory. 
    The client can then decide whether to grab another file or exit. If the filename already exists on the client side, it will be 
    replaced with a new copy. Terminate connection by typing "exit".


Sample Client Output:
    dheck@DESKTOP-5M4C4CG: /DavidHeck/client$ ./client
    CLIENT: Socket creation successful.
    CLIENT: Connected to 127.0.0.1:8080
    CLIENT: Enter the filename you wish to download, or type 'exit' to close connection: testA.txt
    CLIENT: Receiveing testA.txt from Server and saving it.
    CLIENT: File received from server!
    CLIENT: Enter the filename you wish to download, or type 'exit' to close connection: long_test.txt
    CLIENT: Receiveing long_test.txt from Server and saving it.
    CLIENT: File received from server!
    CLIENT: Enter the filename you wish to download, or type 'exit' to close connection: test.jpg
    CLIENT: Receiveing test.jpg from Server and saving it.
    CLIENT: File received from server!
    CLIENT: Enter the filename you wish to download, or type 'exit' to close connection: testB.txt
    CLIENT: Receiveing testB.txt from Server and saving it.
    CLIENT: File received from server!
    CLIENT: Enter the filename you wish to download, or type 'exit' to close connection: test.png
    CLIENT: Receiveing test.png from Server and saving it.
    CLIENT: File received from server!
    CLIENT: Enter the filename you wish to download, or type 'exit' to close connection: nonexistantfile
    SERVER: File nonexistantfile not found on server, please try another file.
    CLIENT: Enter the filename you wish to download, or type 'exit' to close connection: exit
    SERVER: Closing connection.
    CLIENT: Exiting...

Sample Server Output:
    dheck@DESKTOP-5M4C4CG: /DavidHeck/server$ ./server
    SERVER: Socket creation successful.
    SERVER: Socket binding successful.
    SERVER: Listening on Port 8080...
    SERVER: Client acceptance successful.
    CLIENT: Requesting testA.txt
    Server: Sending file testA.txt to Client...
    SERVER: File successfully sent to client!
    CLIENT: Requesting long_test.txt
    Server: Sending file long_test.txt to Client...
    SERVER: File successfully sent to client!
    CLIENT: Requesting test.pdf
    CLIENT: Requesting test.jpg
    Server: Sending file test.jpg to Client...
    SERVER: File successfully sent to client!
    CLIENT: Requesting testB.txt
    Server: Sending file testB.txt to Client...
    SERVER: File successfully sent to client!
    CLIENT: Requesting test.png
    Server: Sending file test.png to Client...
    SERVER: File successfully sent to client!
    CLIENT: Requesting nonexistantfile
    SERVER: ERROR file nonexistantfile not found on server.
    CLIENT: Requesting exit
    SERVER: Exiting...
