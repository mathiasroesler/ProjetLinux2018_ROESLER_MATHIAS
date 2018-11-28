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

#include "../include/misc.h"
#include "../include/communication.h"

/* communication.c
 * 
 * Contains the function for communication between client and server.
 *
 * Author: Mathias ROESLER
 * Date: December 2018
*/

void serverCommunication(int client1_sock, int client2_sock)
/* Function called in the server program to set up 
 * communication between two clients.
*/
{
	char buffer_client1[BUFFER_SIZE];
	char buffer_client2[BUFFER_SIZE];

	/* Signal clients that they are connected */
	write(client1_sock, "1", sizeof("1"));
	write(client2_sock, "2", sizeof("2"));

	/* Retrieve and send client names */
	read(client1_sock, buffer_client1, NAME_SIZE);
	write(client2_sock, buffer_client1, NAME_SIZE);
	read(client2_sock, buffer_client2, NAME_SIZE);
	write(client1_sock, buffer_client2, NAME_SIZE);

	while(streq(buffer_client1, strlen(buffer_client1)) != 0 || streq(buffer_client2, strlen(buffer_client2)) != 0)
	{
		read(client1_sock, buffer_client1, BUFFER_SIZE);	// Get message from client 1
		write(client2_sock, buffer_client1, BUFFER_SIZE);	// Send message to client 2
		read(client2_sock, buffer_client2, BUFFER_SIZE);	// Get answer from client 2
		write(client1_sock, buffer_client2, BUFFER_SIZE);	// Send answer to client 1
	}

	printf("Quit\n");
}

void clientCommunication(int peer_sock)
/* Function called in the client program to set up
 * communication between a client and the server.
*/
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
		printf("Connected with %s", buffer);
		printf("Write message \n");

		while(streq(buffer, strlen(buffer)) != 0)
		{
			memset(buffer, 0, sizeof(buffer));	// Clear buffer
			memset(message, 0, sizeof(message));

			printf("%s", name);
			fgets(buffer, BUFFER_SIZE, stdin); 	// Recuperate message from input
			
			strcpy(message, name);
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
		printf("Connected with %s", buffer);
		printf("Wait for message\n");

		while(streq(buffer, strlen(buffer)) != 0)
		{
			read(peer_sock, buffer, BUFFER_SIZE); 	// Recieve message
			printf("%s", buffer);

			memset(buffer, 0, sizeof(buffer));	// Clear buffer
			memset(message, 0, sizeof(message));
			
			printf("%s", name);
			fgets(buffer, BUFFER_SIZE, stdin); 	// Recuperate message from input
			
			strcpy(message, name);
			strcat(message, buffer);		// Prepare message for sending

			write(peer_sock, message, BUFFER_SIZE); // Send message
		}
	}
}
