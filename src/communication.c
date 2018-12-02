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
 * Contains the function for communication between client and server.
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
	char client1_name[NAME_SIZE];
	char client2_name[NAME_SIZE];
	char client1_buffer[BUFFER_SIZE];
	char client2_buffer[BUFFER_SIZE];
	int nb1, nb2;

	signal(SIGUSR1, sigHandlerChild); // Set trap for signal SIGUSR1

	/* Signal clients that they are connected */
	write(client1_sock, "1", sizeof("1"));
	write(client2_sock, "2", sizeof("2"));


	/* Retrieve and send client names */
	nb1=read(client1_sock, client1_name, NAME_SIZE);

	if (nb1 == 0)
	/* If client1 exited prematurely, end connection */
	{
		write(client2_sock, "exit\n", NAME_SIZE);
		return -1;
	}

	write(client2_sock, client1_name, NAME_SIZE);
	nb2=read(client2_sock, client2_name, NAME_SIZE);

	if (nb2 == 0)
	/* If client2 exited prematurely, end connection */
	{
		write(client1_sock, "exit\n", NAME_SIZE);
		return -1;
	}

	write(client1_sock, client2_name, NAME_SIZE);

	while(1)
	{
		read(client1_sock, client1_buffer, BUFFER_SIZE);	// Get message from client 1

		if (SIGNAL == 1)
		/* If server is closing */
		{
			/* Send closing message to clients */
			write(client2_sock, "close", BUFFER_SIZE);
			write(client1_sock, "close", BUFFER_SIZE);
			break;
		}

		if (streqExit(client1_buffer, strlen(client1_buffer)) == 0)
		/* If client1 sends the message exit */
		{
			write(client2_sock, client1_buffer, BUFFER_SIZE);
			break;
		}

		write(client2_sock, client1_buffer, BUFFER_SIZE);	// Send message to client 2
		
		read(client2_sock, client2_buffer, BUFFER_SIZE);	// Get answer from client 2

		if (SIGNAL == 1)
		/* If server is closing */
		{
			/* Send closing message to clients */
			write(client2_sock, "close", BUFFER_SIZE);
			write(client1_sock, "close", BUFFER_SIZE);
			break;
		}

		if (streqExit(client2_buffer, strlen(client1_buffer)) == 0)
		/* If client2 sends the message exit */
		{
			write(client1_sock, client2_buffer, BUFFER_SIZE);
			break;
		}

		write(client1_sock, client2_buffer, BUFFER_SIZE);	// Send answer to client 1
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
	char client_num[1];
	int name_size=0, num=0;

	printf("Welcome to the message service.\n");

	printf("\nPlease be patient, we are waiting for another client...\n");
	read(peer_sock, client_num, sizeof("1")); 	// Wait until another client is connected
	num = atoi(client_num);

	printf("Thank you for waiting, please enter your name: ");
	fgets(name, NAME_SIZE, stdin);			// Get client name
	write(peer_sock, name, NAME_SIZE); 		// Sends client name to other client

	/* Resize name */
	name_size = strlen(name);  
	name[name_size-1] = '\0';
	strcat(name, ": ");

	if (num == 1)
	/* If first client then send first message */
	{
		read(peer_sock, buffer, NAME_SIZE);		// Get other client name

		if (streqExit(buffer, strlen(buffer)) == 0)
		/* If client2 exits prematurely, end connection */
		{
			printf("Error with connection, please try to connect again.\n");
			return -1;
		}

		else 
		{
			printf("\nYou are now connected with %s", buffer);
			printf("To quit, type exit.\n");
			printf("Please write a message.\n\n");
		}

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
			read(peer_sock, buffer, BUFFER_SIZE); 	// Recieve message

			if (streqClose(buffer, strlen(buffer)) == 0)
			{
				printf("\nThe server is closing, the connection has been terminated.\n");
				write(peer_sock, "close", BUFFER_SIZE);
				break;
			}

			else if (streqExit(buffer, strlen(buffer)) == 0)
			/* If message is exit */
			{
				printf("The connection was terminated by other client.\n");
				break;
			}

			printf("> %s", buffer);	
		}
	}

	else 
	/* If second client wait until message recieved */
	{
		read(peer_sock, buffer, NAME_SIZE);		// Get other client name

		if (streqExit(buffer, strlen(buffer)) == 0)
		/* If client1 exits prematurely, end connection */
		{
			printf("Error with connection, please try to connect again.\n");
			return -1;
		}
		
		else 
		{
			printf("\nYou are now connected with %s", buffer);
			printf("To quit, type exit.\n");
			printf("Please wait for a message.\n\n");
		}

		while(1)
		{
			read(peer_sock, buffer, BUFFER_SIZE); 	// Recieve message

			if (streqClose(buffer, strlen(buffer)) == 0)
			{
				printf("\nThe server is closing, the connection has been terminated.\n");
				write(peer_sock, "close", BUFFER_SIZE);
				break;
			}

			else if (streqExit(buffer, strlen(buffer)) == 0)
			/* If message is exit */
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
