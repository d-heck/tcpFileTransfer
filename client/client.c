// @Author: David Heck (dheck@udel.edu)
// Client Side Implementation Of TCP File Transfer

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
	char fileName[MAX]; // local storage for filename

	// Infinite loop for communication between client and server
	while (1){
		// Reset messageBuffer
		bzero(messageBuffer, sizeof(messageBuffer));

		// Get user input for file name, send input to server
		printf("CLIENT: Enter the filename you wish to download, or type 'exit' to close connection: "); 
		scanf("%s", messageBuffer);	
		memcpy(fileName, messageBuffer, sizeof(messageBuffer)); // Keep local copy of (potential) filename
		write(sockfd, messageBuffer, sizeof(messageBuffer)); // Write message to server

		// If message sent is "exit" wait for server to respond with exit message.
		if ((strncmp(messageBuffer, "exit", 4)) == 0) {
			// Reset messageBuffer, read returned exit message from server
			bzero(messageBuffer, sizeof(messageBuffer)); 
        	read(sockfd, messageBuffer, sizeof(messageBuffer)); // Read message returned from server

			// If server message is "exit", close connection
			if ((strncmp(messageBuffer, "exit", 4)) == 0) {
				printf("SERVER: Closing connection. \n");
				printf("CLIENT: Exiting...\n"); 
				break;
			}
			else{
				printf("CLIENT: ERROR Server did not acknowledge exit. Force closing connection... \n");
				exit(0);
			}
		}

		// Reset messageBuffer, wait for OK message to confirm file existance.
		bzero(messageBuffer, sizeof(messageBuffer)); 
        read(sockfd, messageBuffer, sizeof(messageBuffer)); // Read message returned from server

		// If server finds file and sends OK message, send an OK back and start receiving the file
		if ((strncmp(messageBuffer, "OK", 2)) == 0) {
			// Send ok message back to client
			write(sockfd, messageBuffer, sizeof(messageBuffer)); 

			// Receive File from Server
			printf("CLIENT: Receiveing %s from Server and saving it. \n", fileName);

			// Create file in write mode
			FILE *clientFile = fopen(fileName, "w"); 

			// If the file is null something went wrong, else download file from server
			if(clientFile == NULL){
				printf("CLIENT: ERROR File %s cannot be opened. \n", fileName);
			}
			else{
				// Reset Buffer, init block size
				bzero(messageBuffer, MAX); 
				int blockSize = 0;
				
				// While still receiving (downloading) the file
				while((blockSize = recv(sockfd, messageBuffer, MAX, 0)) > 0){
					
					int fileWrite = fwrite(messageBuffer, sizeof(char), blockSize, clientFile);
					
					// If the fileWrite size is less than the block size
					if(fileWrite < blockSize){
						printf("CLIENT: ERROR File write failed.\n");
					}

					// Reset buffer
					bzero(messageBuffer, MAX);

					// Break out of loop if last block.
					if (blockSize == 0 || blockSize != 512){
						break;
					}
				}

				// Display success message and close File
				printf("CLIENT: File received from server! \n");
				fclose(clientFile);
			}
		}
		else{
			// Else if no OK is received file is not on server.
			printf("SERVER: File %s not found on server, please try another file. \n", fileName);
		}
	}
	// Close Connection
	close(sockfd);
}

// Main Function w/ Argument Fields -- Establishes TCP Connection to Specified IP + Port
int main(int argc, char* argv[]){
    // Init Variables
    char* ipAddress; // Destination IP Address
    char* port; // Destination Port
	int portVal; // Used to convert PORT to an int
	int clientSocket; // Socket for the client
	struct sockaddr_in serverAddress; // Socket address for server

    // Set defaults based on how many arguments are supplied. (e.g. set port to 8080 if only IP is passed in)
    if (argv[1] == NULL){
        ipAddress = "127.0.0.1";
        port = "8080";
    }
	else if(argv[2] == NULL){
        ipAddress = argv[1];
        port = "8080";
    }
	else{
        ipAddress = argv[1];
        port = argv[2]; 
    }

	// Convert PORT string to int value.
    portVal = atoi(port);

	// Create the client's socket, and check for successful creation
	clientSocket = socket(AF_INET, SOCK_STREAM, 0); 
	if (clientSocket == -1) { 
		printf("CLIENT: Socket creation failed. \n"); 
		exit(0); 
	} 
	else{
		printf("CLIENT: Socket creation successful. \n"); 
	}

	// Allocate space for server address.
	bzero(&serverAddress, sizeof(serverAddress)); 

	// Assign the IP and Port Number for the destination address.
	serverAddress.sin_family = AF_INET; 
	serverAddress.sin_addr.s_addr = inet_addr(ipAddress); 
	serverAddress.sin_port = htons(portVal); 

	// Attempt to connect to the server, TCP style. 
	if (connect(clientSocket, (SA*)&serverAddress, sizeof(serverAddress)) != 0) { 
		printf("CLIENT: Connection with the server failed. \n"); 
		exit(0); 
	} 
	else{
		printf("CLIENT: Connected to %s:%d \n", ipAddress, portVal);
	}

	// Initiate file transfer program.
	fileTransfer(clientSocket); 

	// Close Client Socket (end program).
	close(clientSocket); 

    return 0; // End Main
} 