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

int server_connection(int sock, int nb_connection)
/* Sets up the server */
{
	int peer_sock;
	int id;
	struct sockaddr_in address;
	socklen_t length=sizeof(struct sockaddr_in);
	pid_t child_pid;
	cli_t* cli;
	
	
	listen(sock, nb_connection);
	printf("Waiting for connection...\n");
	
	while (1)
	{
		peer_sock = accept(sock, (struct sockaddr*) &address, &length);

		if (peer_sock == -1)
		{
			close(peer_sock);
			printf("Unable to accept connection");
			perror("Error");
			return -1;
		}
	
		printf("Connection succesful with\n");
		print_peer_info(peer_sock);

		child_pid = fork();

		if (child_pid == -1)
		{
			printf("Unable to call the function fork\n");
			perror("Error");
			return -1;
		}

		else if (child_pid == 0)
		/* Child */
		{
			close(sock);
			server_communication(peer_sock);
			return 0;
		}

		else
		/* Father */
		{
			close(peer_sock);
		}
	}

	return 0;
}

void server_communication(int peer_sock)
/* Server communication */
{
	char buffer[BUFFER_SIZE];

	printf("Received messages:\n");

	while(streq(buffer, strlen(buffer)) != 0)
	{
		read(peer_sock, buffer, BUFFER_SIZE);
		printf("%s", buffer);
	}
}

int client_connection(char *ip, char *port)
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

	memset(&address, 0, length); 		   // Fill address with 0
	address.sin_family = AF_INET; 		   // Set family
	address.sin_port = htons(atoi(port));		   // Set port
	inet_aton(ip, &address.sin_addr); // Set ip address
	
	if (connect(sock, (struct sockaddr*) &address, length) == -1)
	{
		close(sock);
		printf("Unable to connect socket %d\n", sock);
		perror("Error");
		return -1;
	}

	return sock;

}	

void client_communication(int peer_sock)
/* Client communication */
{
	char buffer[BUFFER_SIZE];

	printf("Write messages:\n");

	while(streq(buffer, strlen(buffer)) != 0)
	{
		fgets(buffer, BUFFER_SIZE, stdin);
		write(peer_sock, buffer, BUFFER_SIZE);
	}
}
