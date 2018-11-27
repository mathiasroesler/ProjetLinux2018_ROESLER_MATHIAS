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

// misc.c
// Fonction utiles

int printSocketInfo(int sock)
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

int printPeerInfo(int sock)
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

	printf("IP address : %s\n", inet_ntoa(address.sin_addr));
	printf("Port : %u\n\n", ntohs(address.sin_port));
	
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

void setNonBlocking(int sock)
/* Sets a socket to non-blocking mode */
{
	int flags=0;

	flags = fcntl(sock, F_GETFL, 0);

	if (flags != -1)
	{
		fcntl(sock, F_SETFL, flags | O_NONBLOCK);
	}
}
