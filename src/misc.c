#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <string.h>

#include <arpa/inet.h>
#include <netdb.h>
#include <netinet/in.h>

#include <sys/socket.h>

#include "../include/misc.h"

/* misc.c
 *
 * Contains miscellanous function that are used
 * by the client or the server.
 *
 * Author: Mathias ROESLER
 * Date: December 2018
*/

int printSocketInfo(int sock)
/* Prints the socket information, IP address and port number. */
{
	struct sockaddr_in address;
	socklen_t length=sizeof(struct sockaddr_in);

	if (getsockname(sock, (struct sockaddr*) &address, &length) == -1)
	/* Retrieve socket information and store it in address */
	{
		printf("Unable to retrieve the information of socket %d\n", sock);
		perror("Error");
		return -1;
	}

	printf("IP address : %s\n", inet_ntoa(address.sin_addr));
	printf("Port : %u\n\n", ntohs(address.sin_port));

	return 0;
}

int printPeerInfo(int sock)
/* Prints the peer information, IP address and port number. */
{
	struct sockaddr_in address;
	socklen_t length=sizeof(struct sockaddr_in);

	if (getpeername(sock, (struct sockaddr*) &address, &length) == -1)
	/* Retrieve socket information and store it in address */
	{
		printf("Unable to retrieve the information of peer %d\n", sock);
		perror("Error");
		return -1;
	}

	printf("IP address : %s\n", inet_ntoa(address.sin_addr));
	printf("Port : %u\n\n", ntohs(address.sin_port));
	
	return 0;
}


int streqExit(char str[], int str_length)
/* Compares str with the exit. */
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

int streqClose(char str[], int str_length)
/* Compares str with the exit. */
{
	char str_close[]="close";
	int close_length=strlen(str_close);

	if (str_length != close_length)
	{
		return -1;
	}

	for (int i=0; i<str_length; ++i)
	{
		if (str[i] != str_close[i])
		{
			return -1;
		}
	}

	return 0;
}
