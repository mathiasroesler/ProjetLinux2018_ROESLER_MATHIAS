#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <string.h>
#include <signal.h>
#include <errno.h>

#include <arpa/inet.h>
#include <netdb.h>
#include <netinet/in.h>

#include <sys/socket.h>
#include <sys/shm.h>
#include <sys/stat.h>

#include "../include/create.h"
#include "../include/misc.h"
#include "../include/communication.h"

#define MAX_CONNECTION 4

int PID_ARRAY[MAX_CONNECTION/2];
int NB_CONNECTION;

/* create.c
 *
 * Contains the function that create the objects 
 * necessary for the client and the server.
 *
 * Author: Mathias ROESLER
 * Date: December 2018
*/

int createSocket(char *ip, char *port, int flag)
/* Function called to create a socket with a certain
 * ip address and a certain port number.
 * The socket can either be bound or connected
 * depending on the value of flag.
*/
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

	memset(&address, 0, length);		// Fill address with 0
	address.sin_family = AF_INET; 		// Set family
	address.sin_port = htons(atoi(port));	// Set port
	inet_aton(ip, &address.sin_addr); 	// Set ip address

	if (flag == BIND)
	{
		if (bind(sock, (struct sockaddr*) &address, length) == -1)
		// Check the binding of the socket
		{
			close(sock);
			printf("Unable to bind socket %d\n", sock);
			perror("Error");
			return -1;
		}
	}

	else if (flag == CONNECT)
	{
		if (connect(sock, (struct sockaddr*) &address, length) == -1)
		// Check the connection of the socket
		{
			close(sock);
			printf("Unable to connect socket %d\n", sock);
			perror("Error");
			return -1;
		}
	}

	return sock;
}

void sigHandler(int sig_nb) 
{
	if (sig_nb == -1)
	{
		printf("Signal unrecongnized\n");
		perror("Error");
		exit(1);
	}

	else if (sig_nb == SIGUSR2)
	/* User signal 2, prints number of connections to server */
	{
		signal(SIGUSR2, sigHandler);

		if (NB_CONNECTION == 1)
		{
			printf("There is %d client connected\n", NB_CONNECTION);
		}
		
		else
		{
			printf("There are %d clients connected\n", NB_CONNECTION);
		}
	}

	else if (sig_nb == SIGUSR1)
	/* User signal 1, closes all the connections */
	{
		signal(SIGUSR1, sigHandler);
		kill(PID_ARRAY[0], SIGUSR1);
	}
}

void sigHandlerChild(int sig_nb)
{
	if (sig_nb == SIGUSR1)
	{
		printf("Received\n");
	}
}

void createServer(int sock)
/* Function called to create a server. */
{
	int peer_sock=0;
	int peer_sock_mem[MAX_CONNECTION];
	struct sockaddr_in address;
	socklen_t length=sizeof(struct sockaddr_in);
	pid_t child_pid;
	
	listen(sock, 1);

	signal(SIGUSR1, sigHandler);
	signal(SIGUSR2, sigHandler);

	while (1)
	{
		do
		/* Wait for two clients to connect */
		{
			peer_sock = accept(sock, (struct sockaddr*) &address, &length);

			if (peer_sock == -1)
			/* Check if connection is refused */
			{
				close(peer_sock);
				printf("Unable to accept connection\n");
				perror("Error");
				exit(1);
			}

			peer_sock_mem[NB_CONNECTION] = peer_sock;	// Store client socket number
			++NB_CONNECTION;				

		} while (NB_CONNECTION%2 != 0);

		child_pid = fork();
		
		if (child_pid == -1)
		/* Check if call to fork is unsuccesful */
		{
			close(peer_sock);
			printf("Unable to call function fork\n");
			perror("Error");
			exit(1);
		}

		else if (child_pid == 0)
		/* Child */
		{
			signal(SIGUSR1, sigHandlerChild);
			close(sock);
			serverCommunication(peer_sock_mem[NB_CONNECTION-2], peer_sock_mem[NB_CONNECTION-1]);
			exit(0);
		}

		else
		/* Father */
		{
			close(peer_sock);
			PID_ARRAY[(NB_CONNECTION/2)-1] = child_pid; // Store child PID
		}
	}
}

