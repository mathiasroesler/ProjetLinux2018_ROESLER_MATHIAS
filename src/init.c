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

#include "../include/init.h"
#include "../include/misc.h"
#include "../include/shm_fct.h"
#include "../include/socket_fct.h"

#define MAX_CONNECTION 4

int PID_ARRAY[MAX_CONNECTION];

// init.c
// Fonction d'initialisation

int createSocket(char *ip, char *port, int flag)
/* Creates a socket */
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
	int i, index=0;

	while (PID_ARRAY[index] != 0)
	{
		++index;
	}

	if (sig_nb == -1)
	{
		printf("Signal unrecongnized\n");
		perror("Error");
		exit(1);
	}

	else if (sig_nb == SIGUSR1)
	{
		signal(SIGUSR1, sigHandler);


		if (index == 1)
		{
			printf("There is %d connection\n", index);
		}
		
		else
		{
			printf("There are %d connections\n", index);
		}
	}

	else if (sig_nb == SIGUSR2)
	{
		signal(SIGUSR2, sigHandler);
	}
}

void initServer(int sock)
/* Initializes the server */
{
	int peer_sock=0;
	int peer_sock_mem[MAX_CONNECTION];
	int i;
	int shm_id=0;
	int index=0;
	int nb_connection=0;
	char buffer[256];
	struct sockaddr_in address;
	socklen_t length=sizeof(struct sockaddr_in);
	pid_t child_pid;
	
	listen(sock, 1);

	while (1)
	{
		do
		{
			peer_sock = accept(sock, (struct sockaddr*) &address, &length);

			if (peer_sock == -1)
			{
				close(peer_sock);
				printf("Unable to accept connection\n");
				perror("Error");
				exit(1);
			}

			peer_sock_mem[nb_connection] = peer_sock;
			++nb_connection;

		} while (nb_connection%2 != 0);

		child_pid = fork();
		
		if (child_pid == -1)
		{
			close(peer_sock);
			printf("Unable to call function fork\n");
			perror("Error");
			exit(1);
		}

		else if (child_pid == 0)
		/* Child */
		{
			close(sock);
			serverCommunication(peer_sock_mem[nb_connection-2], peer_sock_mem[nb_connection-1]);
			exit(0);
		}

		else
		/* Father */
		{
			close(peer_sock);

			printf("pid %d\n", getpid());
			signal(SIGUSR1, sigHandler);
			signal(SIGUSR2, sigHandler);

			PID_ARRAY[(nb_connection/2)-1] = child_pid;
		}
	}
}

