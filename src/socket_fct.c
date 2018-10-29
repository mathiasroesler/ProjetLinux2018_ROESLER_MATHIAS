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

#include "socket_fct.h"

#define BUFFER_SIZE 256

// socket_fct.c
// Contains the function to create a socket, a client and a server

int create_socket(char *ip, char *port)
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

	memset(&address, 0, length); 		   // Fill address with 0
	address.sin_family = AF_INET; 		   // Set family
	address.sin_port = htons(atoi(port));		   // Set port
	inet_aton(ip, &address.sin_addr); // Set ip address

	if (bind(sock, (struct sockaddr*) &address, length) == -1)
	// Check the binding of the socket
	{
		close(sock);
		printf("Unable to bind socket %d\n", sock);
		perror("Error");
		return -1;
	}

	return sock;
}

int print_socket_info(int sock)
/* Prints the socket information, IP address and port number */
{
	struct sockaddr_in address;
	socklen_t length=sizeof(struct sockaddr_in);

	if (getsockname(sock, (struct sockaddr*) &address, &length) == -1)
	{
		printf("Unable to retrieve the information of socket %d\n", sock);
		perror("Error");
		return -1;
	}

	printf("IP address : %s\n", inet_ntoa(address.sin_addr));
	printf("Port : %u\n\n", ntohs(address.sin_port));

	return 0;
}

int print_peer_info(int sock)
/* Prints the peer information, IP address and port number */
{
	struct sockaddr_in address;
	socklen_t length=sizeof(struct sockaddr_in);

	if (getpeername(sock, (struct sockaddr*) &address, &length) == -1)
	{
		printf("Unable to retrieve the information of peer %d\n", sock);
		perror("Error");
		return -1;
	}

	printf("Peer IP address : %s\n", inet_ntoa(address.sin_addr));
	printf("Peer port : %u\n\n", ntohs(address.sin_port));

	return 0;
}

int streq(char str[], int str_length)
/* Compares str with exit */
{
	char str_exit[]="exit\n";
	int exit_length=strlen(str_exit);

	if (str_length != exit_length)
	{
		return -1;
	}

	for (int i=0; i<str_length; ++i)
	{
		if (str[i] != str_exit[i])
		{
			return -1;
		}
	}

	return 0;
}

int server_connection(int sock, int nb_connection)
/* Sets up the server */
{
	int peer_sock;
	struct sockaddr_in address;
	socklen_t length=sizeof(struct sockaddr_in);
	pid_t son_pid;

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

		son_pid = fork();

		if (son_pid == -1)
		{
			printf("Unable to call the function fork\n");
			perror("Error");
			return -1;
		}

		else if (son_pid == 0)
		/* Son */
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
