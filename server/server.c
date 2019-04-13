// @Author: David Heck (dheck@udel.edu)
// Server Side Implementation Of TCP File Transfer

// Includes
#include <stdlib.h> 
#include <stdio.h>
#include <string.h> 
#include <sys/socket.h> 
#include <netdb.h> 
#include <netinet/in.h> 
#include <sys/types.h> 
#include <unistd.h>
#include <arpa/inet.h>

// Constants
#define MAX 512 // Maximum buffer size
#define SA struct sockaddr

// Function called after connection is established, transfers specified files from server to client until server gets an exit message.
void fileTransfer(int sockfd){ 	
	// Init Variables
	char messageBuffer[MAX]; // buffer used to transmit messages between client and server
	char fileName[MAX]; // Used to store a local copy of the file name

	// Infinite loop for communication between client and server
	while(1) { 

		// Reallocate buffer with MAX buffer size
		bzero(messageBuffer, MAX); 

		// Read the message sent from CLIENT, copy to local buffer.
		read(sockfd, messageBuffer, sizeof(messageBuffer));

		// print buffer which contains the client contents 
		printf("CLIENT: Requesting %s \n" , messageBuffer);

		// Terminate connection if client requests to exit.
		if ((strncmp(messageBuffer, "exit", 4)) == 0) {
			// Send exit message backt to client
			write(sockfd, messageBuffer, sizeof(messageBuffer)); 
			printf("SERVER: Exiting...\n"); 
			break; 
		}
	
		// Attempt to open specified file
		memcpy(fileName, messageBuffer, sizeof(messageBuffer)); // Keep local copy of (potential) filename
		FILE* serverFile = fopen(fileName, "r"); // Open file in read mode, store as serverFile

		// If the file exists on the server
		if(serverFile != NULL){
			// Inform client that file is on server via OK message
			write(sockfd, "OK\0", sizeof("OK\0"));

			// Reset messageBuffer, wait for client to acknowledge the OK message
			bzero(messageBuffer, sizeof(messageBuffer)); 
        	read(sockfd, messageBuffer, sizeof(messageBuffer));

			// If read message is OK send file
			if ((strncmp(messageBuffer, "OK", 2)) == 0) {
				int blockSize; // Size of each block being sent
				
				// Reset messageBuffer
				bzero(messageBuffer, sizeof(messageBuffer)); 
				printf("Server: Sending file %s to Client... \n", fileName);
				
				// Continuously send file contents until EOF
				while((blockSize = fread(messageBuffer, sizeof(char), MAX, serverFile)) > 0){
					if(send(sockfd, messageBuffer, blockSize, 0) < 0){
						printf("SERVER: ERROR Failed to send file %s. Closing connection. \n", fileName);
						exit(0);
					}

					// Reset Message Buffer
					bzero(messageBuffer, MAX);
				}
				printf("SERVER: File successfully sent to client! \n");
			}
		}
		else{
			// Else if file not on server -- print error, send NULL back to client. Wait for next filename from client.
			printf("SERVER: ERROR file %s not found on server. \n", fileName);
			write(sockfd, "NULL\0", sizeof("NULL\0")); 
		}
	}

	// Close connection
	close(sockfd);
}

// Main Function w/ Argument Fields -- Establishes TCP Connection to Specified IP + Port
int main(int argc, char* argv[]){
    // Init Variables
    char* port; // Listening Port
    int portVal; // Used to convert PORT to an int
    int serverSocket; // Socket for server
	int clientLength; // Length of client (used for accept)
	int connection; // Used in client acceptance
	struct sockaddr_in serverAddress; // Socket address in for server
	struct sockaddr_in client; // Socket address in for client

    // Set port based command line argument, sets to 8080 if no port specified. 
    if (argv[1] == NULL){
        port = "8080";
    }
	else{
        port = argv[1];
    }

    // Convert PORT string to int value.
    portVal = atoi(port);

	// Create the server's socket, and check for successful creation
	serverSocket = socket(AF_INET, SOCK_STREAM, 0); 
	if (serverSocket == -1) { 
		printf("SERVER: Socket creation failed. \n"); 
		exit(0); 
	} 
	else{
		printf("SERVER: Socket creation successful. \n");
	}

	// Allocate space for server address.
	bzero(&serverAddress, sizeof(serverAddress)); 

	// Assign the IP and Port Number for the Server address.
	serverAddress.sin_family = AF_INET; 
	serverAddress.sin_addr.s_addr = htonl(INADDR_ANY); // Take any IP as in address (accept all clients)
	serverAddress.sin_port = htons(portVal); // Set port (user specified)

	// Bind server socket with server address.
	if ((bind(serverSocket, (SA*)&serverAddress, sizeof(serverAddress))) != 0) { 
		printf("SERVER: Socket binding failed. \n"); 
		exit(0); 
	} 
	else{
		printf("SERVER: Socket binding successful. \n"); 
	}

	// Now server is ready to listen and verification 
	if ((listen(serverSocket, 5)) != 0) { 
		printf("SERVER: Listening on port %d failed. \n", portVal); 
		exit(0); 
	} 
	else{
		printf("SERVER: Listening on Port %d... \n", portVal);
	}

	// Store size of client in clientLength
	clientLength = sizeof(client); 

	// Accept the data packet (message) from client and verifify connection was accepted
	connection = accept(serverSocket, (SA*)&client, &clientLength); 
	if (connection < 0) { 
		printf("SERVER: Client acceptance failed. \n"); 
		exit(0); 
	} 
	else{
		printf("SERVER: Client acceptance successful. \n");
	}

	// Initiate file transfer program.
	fileTransfer(connection); 

	// Close Server Socket (end program)
	close(serverSocket);
    
    return 0; // End Main
}