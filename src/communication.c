#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <string.h>
#include <signal.h>
#include <errno.h>

#include "../include/misc.h"
#include "../include/communication.h"

/* communication.c
 * 
 * Contains the function for communication between 
 * the client and the server.
 *
 * Author: Mathias ROESLER
 * Date: December 2018
*/

int SIGNAL = 0;

int serverCommunication(int client1_sock, int client2_sock)
/* Function called in the server program to set up 
 * communication between two clients.
*/
{
	char client1_buffer[BUFFER_SIZE];
	char client2_buffer[BUFFER_SIZE];

	signal(SIGUSR1, sigHandlerChild); // Set trap for signal SIGUSR1

	/* Signal clients that they are connected */
	write(client1_sock, "1", sizeof("1"));
	write(client2_sock, "2", sizeof("2"));

	if (serverSetup(client1_sock, client2_sock) == -1)
	/* If there is an error while setup */
	{
		return -1;
	}

	while(1)
	/* Communication between the two clients */
	{
		if (serverClientCommunication(client1_sock, client2_sock, client1_buffer) == -1)
		/* If the communication is terminated */
		{
			break;
		}

		if (serverClientCommunication(client2_sock, client1_sock, client2_buffer) == -1)
		/* If the communication is terminated */
		{
			break;
		}
	}

	return 0;
}

int clientCommunication(int peer_sock)
/* Function called in the client program to set up
 * communication between a client and the server.
*/
{
	char name[NAME_SIZE];
	char buffer[BUFFER_SIZE];
	char message[BUFFER_SIZE];
	int  num=0; // Client number

	num = clientSetup(peer_sock, buffer, name);

	if (num == -1)
	/* If there is an error while setup */
	{
		return -1;
	}

	if (num == 1)
	/* First connected client */
	{
		while(1)
		{
			/* Clear buffers */
			memset(buffer, 0, sizeof(buffer));
			memset(message, 0, sizeof(message));

			printf("> %s", name);
			fgets(buffer, BUFFER_SIZE, stdin); 	// Recuperate message from input

			if (streqExit(buffer, strlen(buffer)) == 0)
			/* If message is exit */
			{
				write(peer_sock, buffer, BUFFER_SIZE);
				break;
			}
			
			/* Prepare message for sending */
			strcpy(message, name);
			strcat(message, buffer);

			write(peer_sock, message, BUFFER_SIZE); // Send message
			read(peer_sock, buffer, BUFFER_SIZE);	// Recieve message

			if (clientExitHandler(peer_sock, buffer) == -1)
			/* If the communication is terminated */
			{
				break;
			}

			printf("> %s", buffer);	
		}
	}

	else 
	/* Second client connected */
	{
		while(1)
		{
			read(peer_sock, buffer, BUFFER_SIZE); 	// Recieve message

			if (clientExitHandler(peer_sock, buffer) == -1)
			/* If the communication is terminated */
			{
				break;
			}

			printf("> %s", buffer);

			/* Clear buffers */
			memset(buffer, 0, sizeof(buffer));
			memset(message, 0, sizeof(message));
			
			printf("> %s", name);
			fgets(buffer, BUFFER_SIZE, stdin); 	// Recuperate message from input
			
			if (streqExit(buffer, strlen(buffer)) == 0)
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

	return 0;
}

int serverClientCommunication(int sender_sock, int reader_sock, char sender_buffer[])
/* Handles the exchange of messages between two clients via the server.
 * The different exit cases are also handled.
*/ 
{
	int nb_char=0; // Numbers of characters read

	nb_char = read(sender_sock, sender_buffer, BUFFER_SIZE);	// Get message from sender

	if (SIGNAL == 1)
	/* If the server is closing */
	{
		/* Send closing message to clients */
		write(reader_sock, "close", BUFFER_SIZE);
		write(sender_sock, "close", BUFFER_SIZE);
		return -1;
	}

	else if (nb_char == 0)
	/* If the sender socket is closed */
	{
		write(reader_sock, "disconnected", BUFFER_SIZE);
		return -1;
	}

	else if (streqExit(sender_buffer, strlen(sender_buffer)) == 0)
	/* If the sender sends the message exit */
	{
		write(reader_sock, sender_buffer, BUFFER_SIZE);
		return -1;
	}

	write(reader_sock, sender_buffer, BUFFER_SIZE);	// Send message to reader

	return 0;
}

int clientExitHandler(int peer_sock, char buffer[])
/* Handles the different exit cases for the client. */
{

	if (streqDisconnected(buffer, strlen(buffer)) == 0)
	/* If the socket was closed */
	{
		printf("\nError with the other client connection, you will be disconnected.\n");
		return -1;
	}

	else if (streqClose(buffer, strlen(buffer)) == 0)
	/* If the server is closing */
	{
		printf("\nThe server is closing, the connection has been terminated.\n");
		write(peer_sock, "close", BUFFER_SIZE);
		return -1;
	}

	else if (streqExit(buffer, strlen(buffer)) == 0)
	/* If the message is exit */
	{
		printf("The connection was terminated by other client.\n");
		return -1;
	}

	return 0;
}

int clientSetup(int peer_sock, char buffer[], char name[])
/* Handles the client interface setup. */
{

	char client_num[1];
	int name_size=0, num=0; // Size of client name, client number

	printf("Welcome to the messaging service.\n");

	printf("\nPlease be patient, we are waiting for another client...\n");
	read(peer_sock, client_num, sizeof("1")); 	// Wait until another client is connected
	num = atoi(client_num);

	printf("Thank you for waiting, please enter your name: ");
	fgets(name, NAME_SIZE, stdin);			// Get client name
	write(peer_sock, name, NAME_SIZE); 		// Sends client name to other client
	printf("Waiting for the other client to enter his name...\n");

	/* Resize name */
	name_size = strlen(name);  
	name[name_size-1] = '\0';
	strcat(name, ": ");

	if (num == 1)
	/* If first client then send first message */
	{
		read(peer_sock, buffer, NAME_SIZE);	// Get other client name

		if (streqExit(buffer, strlen(buffer)) == 0)
		/* If other client exits prematurely, end connection */
		{
			printf("Error with the other client connection, please try to connect again.\n");
			return -1;
		}
	
		else 
		{
			printf("\nYou are now connected with %s", buffer);
			printf("To quit, type exit.\n");
			printf("Please write a message.\n\n");
		}
	}

	else
	/* If second client wait until message recieved */
	{
		read(peer_sock, buffer, NAME_SIZE);	// Get other client name

		if (streqExit(buffer, strlen(buffer)) == 0)
		/* If other client exits prematurely, end connection */
		{
			printf("Error with the other client connection, please try to connect again.\n");
			return -1;
		}
	
		else 
		{
			printf("\nYou are now connected with %s", buffer);
			printf("To quit, type exit.\n");
			printf("Please wait for a message.\n\n");
		}
	}

	return num;
}

int serverSetup(int client1_sock, int client2_sock)
/* Handles the server setup. */
{
	char client1_name[NAME_SIZE];
	char client2_name[NAME_SIZE];
	int nb_char=0;	// Numbers of characters read

	nb_char=read(client1_sock, client1_name, NAME_SIZE); 	// Read name from client 1

	if (nb_char == 0)
	/* If client1 exited prematurely, end connection */
	{
		write(client2_sock, "exit\n", NAME_SIZE);
		return -1;
	}

	write(client2_sock, client1_name, NAME_SIZE);		// Write name of client 1 to client 2
	nb_char=read(client2_sock, client2_name, NAME_SIZE);	// Read nam from client 2

	if (nb_char == 0)
	/* If client2 exited prematurely, end connection */
	{
		write(client1_sock, "exit\n", NAME_SIZE);
		return -1;
	}

	write(client1_sock, client2_name, NAME_SIZE);		// Write name of client 2 to client 1

	return 0;
}

void sigHandlerChild(int sig_num)
/* Signal handler function for SIGUSR1.
 * Changes the value of SIGNAL to start the
 * closing routine.
*/
{
	if (sig_num == -1)
	{
		printf("Signal unrecognized\n");
		perror("Error");
		exit(1);
	}

	else if (sig_num == SIGUSR1)
	{
		SIGNAL = 1;
	}
}
