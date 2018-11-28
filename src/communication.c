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
	char client1_buffer[BUFFER_SIZE];
	char client2_buffer[BUFFER_SIZE];

	/* Signal clients that they are connected */
	write(client1_sock, "1", sizeof("1"));
	write(client2_sock, "2", sizeof("2"));

	/* Retrieve and send client names */
	read(client1_sock, client1_buffer, NAME_SIZE);
	write(client2_sock, client1_buffer, NAME_SIZE);
	read(client2_sock, client2_buffer, NAME_SIZE);
	write(client1_sock, client2_buffer, NAME_SIZE);

	while(1)
	{
		read(client1_sock, client1_buffer, BUFFER_SIZE);	// Get message from client 1

		if (streq(client1_buffer, strlen(client1_buffer)) == 0)
		/* If client1 sends the message exit */
		{
			write(client2_sock, client1_buffer, BUFFER_SIZE);
			break;
		}

		write(client2_sock, client1_buffer, BUFFER_SIZE);	// Send message to client 2
		read(client2_sock, client2_buffer, BUFFER_SIZE);	// Get answer from client 2

		if (streq(client2_buffer, strlen(client1_buffer)) == 0)
		/* If client2 sends the message exit */
		{
			write(client1_sock, client2_buffer, BUFFER_SIZE);
			break;
		}

		write(client1_sock, client2_buffer, BUFFER_SIZE);	// Send answer to client 1
	}
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

	printf("Welcome, please enter your name:\n");
	fgets(name, BUFFER_SIZE, stdin);		// Get client name

	printf("\nThank you, waiting for another client...\n");
	read(peer_sock, client_num, sizeof("1")); 	// Wait until another client is connected
	num = atoi(client_num);

	write(peer_sock, name, NAME_SIZE); 		// Sends client name to other client

	/* Resize name */
	name_size = strlen(name);  
	name[name_size-1] = '\0';
	strcat(name, ": ");

	if (num == 1)
	/* If first client then send first message */
	{
		read(peer_sock, buffer, NAME_SIZE);		// Get other client name
		printf("\nConnected with %s", buffer);
		printf("To quit, type exit.\n");
		printf("Please write a message.\n\n");

		while(1)
		{
			/* Clear buffers */
			memset(buffer, 0, sizeof(buffer));
			memset(message, 0, sizeof(message));

			printf("> %s", name);
			fgets(buffer, BUFFER_SIZE, stdin); 	// Recuperate message from input

			if (streq(buffer, strlen(buffer)) == 0)
			/* If message is exit */
			{
				write(peer_sock, buffer, BUFFER_SIZE);
				exit(0);
			}
			
			/* Prepare message for sending */
			strcpy(message, name);
			strcat(message, buffer);

			write(peer_sock, message, BUFFER_SIZE); // Send message
			read(peer_sock, buffer, BUFFER_SIZE); 	// Recieve message

			if (streq(buffer, strlen(buffer)) == 0)
			{
				printf("The connection was terminated by other client.\n");
				exit(0);
			}

			printf("> %s", buffer);			
		}
	}

	else 
	/* If second client wait until message recieved */
	{
		read(peer_sock, buffer, NAME_SIZE);		// Get other client name
		printf("\nConnected with %s", buffer);
		printf("To quit, type exit.\n");
		printf("Please wait for a message.\n\n");

		while(1)
		{
			read(peer_sock, buffer, BUFFER_SIZE); 	// Recieve message

			if (streq(buffer, strlen(buffer)) == 0)
			{
				printf("The connection was terminated by other client.\n");
				break;
			}

			printf("> %s", buffer);

			/* Clear buffers */
			memset(buffer, 0, sizeof(buffer));
			memset(message, 0, sizeof(message));
			
			printf("> %s", name);
			fgets(buffer, BUFFER_SIZE, stdin); 	// Recuperate message from input
			
			if (streq(buffer, strlen(buffer)) == 0)
			/* If message is exit */
			{
				write(peer_sock, buffer, BUFFER_SIZE);
				break;
			}

			/* Prepare message for sending */
			strcpy(message, name);
			strcat(message, buffer);

			write(peer_sock, message, BUFFER_SIZE); // Send message
		}
	}
}
