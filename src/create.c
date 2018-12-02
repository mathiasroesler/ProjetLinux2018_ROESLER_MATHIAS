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
#include <sys/wait.h>
#include <sys/stat.h>

#include "../include/create.h"
#include "../include/misc.h"
#include "../include/communication.h"

/* create.c
 *
 * Contains the function that create the objects 
 * necessary for the client and the server.
 *
 * Author: Mathias ROESLER
 * Date: December 2018
*/

int PID_ARRAY[MAX_CONNECTION/2];
int NB_CONNECTION;

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

void createServer(int sock)
/* Function called to create a server. */
{
	int peer_sock=0, i=0;
	int peer_sock_mem[MAX_CONNECTION];
	struct sockaddr_in address;
	socklen_t length=sizeof(struct sockaddr_in);
	pid_t child_pid;
	
	listen(sock, 1);

	/* Set traps for different signals */
	signal(SIGUSR1, sigHandler);
	signal(SIGUSR2, sigHandler);
	signal(SIGTSTP, sigHandler);

	while (1)
	{
		do
		/* Wait for two clients to connect */
		{
			while(NB_CONNECTION == MAX_CONNECTION); // Wait until connection is available

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
			/* Close all  connections */
			for (i=0; i<MAX_CONNECTION; ++i)
			{
				close(peer_sock_mem[i]);
			}
			close(sock);

			printf("Unable to call function fork\n");
			perror("Error");
			exit(1);
		}

		else if (child_pid == 0)
		/* Child */
		{
			close(sock);	// Close unused connection
			serverCommunication(peer_sock_mem[NB_CONNECTION-2], peer_sock_mem[NB_CONNECTION-1]);
			kill(getppid(), SIGTSTP); // Signal server that task is done

			for (i=0; i<MAX_CONNECTION; ++i)
			/* Close all connections */
			{
				close(peer_sock_mem[i]);
			}
			break;
		}

		else
		/* Father */
		{
			/* Close unused connections */
			close(peer_sock_mem[NB_CONNECTION-2]);
			close(peer_sock_mem[NB_CONNECTION-1]);

			PID_ARRAY[(NB_CONNECTION/2)-1] = child_pid; // Store child PID
		}
	}
}

void sigHandler(int sig_nb) 
/* Signal handler function for different signals that can
 * be sent to the father.
*/
{
	int status=0, i=0;

	if (sig_nb == -1)
	{
		printf("Signal unrecongnized\n");
		perror("Error");
		exit(1);
	}

	switch (sig_nb)
	{

	case SIGTSTP:
	/* Reduce number of connections when a child is done */
		signal(SIGTSTP, sigHandler);	// Reset trap for SIGUSR1

		NB_CONNECTION -= 2;
		break;

	case SIGUSR2:
	/* Print the number of connections to the server */
		signal(SIGUSR2, sigHandler);	// Reset trap for SIGUSR2

		if (NB_CONNECTION == 1)
		{
			printf("There is %d client connected\n", NB_CONNECTION);
		}
		
		else
		{
			printf("There are %d clients connected\n", NB_CONNECTION);
		}
		break;

	case SIGUSR1:
	/* Terminated the server and warn children */	
		for (i=0; i<MAX_CONNECTION/2; ++i)
		{
			kill(PID_ARRAY[i], SIGUSR1);
			wait(&status);
		}

		kill(getpid(), SIGTERM);
		break;
	}
}
