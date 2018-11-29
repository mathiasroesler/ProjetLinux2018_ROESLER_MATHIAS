#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#include "include/misc.h"
#include "include/create.h"
#include "include/communication.h"

/* client.c
 *
 * Client end for a client/server communication program.
 *
 * usage: ./client [ip address] [port]
 *
 * Author: Mathias ROESLER
 * Date: December 2018
*/

int main(int argc, char* argv[])
{
	int sock;

	if (argc != 3)
	/* Check if usage is proper */
	{
		printf("usage: ./client [ip address] [port]\n");
		exit(1);
	}

	sock = createSocket(argv[1], argv[2], CONNECT); // Creates a socket and connects it

	if (sock == -1)
	/* Check if socket is create properly */
	{
		close(sock);
		exit(1);
	}

	//printSocketInfo(sock);	// Prints the clients socket information
	clientCommunication(sock);	// Starts the communication with the server
	printf("Have a nice day.\n");
	
	close(sock);
	exit(0);
}
