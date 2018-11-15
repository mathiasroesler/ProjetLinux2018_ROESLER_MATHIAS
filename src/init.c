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
#include <sys/shm.h>

#include "../include/init.h"
#include "../include/misc.h"
#include "../include/socket_fct.h"

// init.c
// Fonction d'initialisation

int create_socket(char *ip, char *port, int flag)
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

int create_shm()
/* Creates a shared memory */
{
	int id;			  // Shared memory id
	int size=4*sizeof(cli_t); // Shared memory size
	key_t key;		  // Shared memory key

	key = ftok("init.c", 1);

	id = shmget(key, size, 0666|IPC_CREAT|IPC_EXCL);

	if (id == -1)
	// Error while creating shared memory
	{
		if (errno == EEXIST)
		// Shared memory exists
		{
			id = shmget(key, size, 0666|IPC_CREAT);
			printf("Shared memory opened with id %d\n", id);
			return id;
		}

		else
		{
			printf("Unable to create shared memory\n");
			perror("Error");
			return -1;
		}
	}

	else
	{
		printf("Shared memory created with id %d\n");
		return id;
	}
}

void init_server(int sock, int nb_connection)
/* Initializes the server */
{
	int peer_sock;
	int shm_id;
	struct sockaddr_in address;
	socklen_t length=sizeof(struct sockaddr_in);
	pid_t child_pid;
	cli_t* cli;
	
	listen(sock, nb_connection);
	printf("Waiting for connection...\n");

	shm_id = create_shm();
	cli = shmat(shm_id, NULL, 0);

	if (cli == (void*)-1)
	{
		printf("Unable to attach segment\n");
		perror("Error");
		exit(1);
	}

	init_shm(cli);

	while (1)
	{
		peer_sock = accept(sock, (struct sockaddr*) &address, &length);

		if (peer_sock == -1)
		{
			close(peer_sock);
			printf("Unable to accept connection\n");
			perror("Error");
			exit(1);
		}
	
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
			fill_client_info(cli[0], peer_sock, 0, "NAME");
			cli[0].socket = peer_sock;
			strcpy(cli[0].name, "NAME");
			//print_client_info(cli[0]);
			server_communication(peer_sock);
			exit(0);
		}

		else
		/* Father */
		{
			print_client_info(cli[0]);
			write(cli[0].socket, "hello", strlen("hello"));
			close(peer_sock);
		}
	}
}

