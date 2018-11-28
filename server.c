#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#include "include/misc.h"
#include "include/create.h"

/* server.c
 *
 * Server end for a client/server communication program.
 *
 * usage: ./server [ip address] [port]
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
		printf("usage: ./server [ip address] [port]\n");
		exit(1);
	}
	
	sock = createSocket(argv[1], argv[2], BIND); // Creates socket and binds it

	if (sock == -1)
	/* Check if socket is created properly */
	{
		close(sock);
		exit(1);
	}

	printSocketInfo(sock);

	createServer(sock); 

	close(sock);
	exit(0);

}
