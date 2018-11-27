#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <string.h>
#include <errno.h>

#include <arpa/inet.h>
#include <netdb.h>
#include <netinet/in.h>

#include <sys/socket.h>

#include "../include/socket_fct.h"
#include "../include/init.h"
#include "../include/misc.h"

#define BUFFER_SIZE 256

// socket_fct.c
// Contains the function to create a socket, a client and a server

void serverCommunication(int client1_sock, int client2_sock)
/* Server communication */
{
	char buffer[BUFFER_SIZE];

	/* Signal clients that they are connected */
	write(client1_sock, "1", sizeof("1"));
	write(client2_sock, "2", sizeof("2"));

	/* Retrieve and send client names */
	read(client1_sock, buffer, NAME_SIZE);
	write(client2_sock, buffer, NAME_SIZE);
	read(client2_sock, buffer, NAME_SIZE);
	write(client1_sock, buffer, NAME_SIZE);

	while(streq(buffer, strlen(buffer)) != 0)
	{
		read(client1_sock, buffer, BUFFER_SIZE);	// Get message from client 1
		write(client2_sock, buffer, BUFFER_SIZE);	// Send message to client 2
		read(client1_sock, buffer, BUFFER_SIZE);	// Get answer from client 2
		write(client2_sock, buffer, BUFFER_SIZE);	// Send answer to client 1
	}
}

int clientConnection(char *ip, char *port)
/* Sets up the client */
{
	int sock;
	struct sockaddr_in address;
	socklen_t length=sizeof(struct sockaddr_in);

	sock = socket(AF_INET, SOCK_STREAM, 0);

	if (sock == -1)
	// Check the creation of the socket
	{
		close(sock);
		printf("Unable to create socket\n");
		perror("Error");
		return -1;
	}

	memset(&address, 0, length); 		// Fill address with 0
	address.sin_family = AF_INET; 		// Set family
	address.sin_port = htons(atoi(port));	// Set port
	inet_aton(ip, &address.sin_addr); 	// Set ip address
	
	if (connect(sock, (struct sockaddr*) &address, length) == -1)
	{
		close(sock);
		printf("Unable to connect socket %d\n", sock);
		perror("Error");
		return -1;
	}

	return sock;

}	

void clientCommunication(int peer_sock)
/* Client communication */
{
	char name[NAME_SIZE];
	char buffer[BUFFER_SIZE];
	char message[BUFFER_SIZE];
	char client_num[1];
	int name_size=0;
	int num=0;

	printf("Welcome, please enter your name\n");
	fgets(name, BUFFER_SIZE, stdin);		// Get client name

	printf("Waiting for another client\n");
	read(peer_sock, client_num, sizeof("1")); 	// Wait until another client is connected
	num = atoi(client_num);

	write(peer_sock, name, NAME_SIZE); 		// Sends client name to other client

	/* Resize name */
	name_size = strlen(name);  
	name[name_size-1] = '\0';
	strcat(name, ": ");
	strcpy(message, name);

	if (num == 1)
	/* If first client then send first message */
	{
		read(peer_sock, buffer, NAME_SIZE);		// Get other client name
		printf("Connected with %s");
		printf("Write message \n");

		while(streq(buffer, strlen(buffer)) != 0)
		{
			memset(buffer, 0, sizeof(buffer));	// Clear buffer

			printf("%s", name);
			fgets(buffer, BUFFER_SIZE, stdin); 	// Recuperate message from input
			
			strcat(message, buffer);		// Prepare message for sending

			write(peer_sock, message, BUFFER_SIZE); // Send message
			read(peer_sock, buffer, BUFFER_SIZE); 	// Recieve message
			printf("%s", buffer);			
		}
	}

	else 
	/* If second client wait until message recieved */
	{
		read(peer_sock, buffer, NAME_SIZE);		// Get other client name
		printf("Connected with %s");
		printf("Wait for message\n");

		while(streq(buffer, strlen(buffer)) != 0)
		{
			read(peer_sock, buffer, BUFFER_SIZE); 	// Recieve message
			printf("%s", buffer);

			memset(buffer, 0, sizeof(buffer));	// Clear buffer
			
			printf("%s", name);
			fgets(buffer, BUFFER_SIZE, stdin); 	// Recuperate message from input

			strcat(message, buffer);		// Prepare message for sending

			write(peer_sock, message, BUFFER_SIZE); // Send message
		}
	}
}
