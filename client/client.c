#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#include "../include/misc.h"
#include "../include/init.h"
#include "../include/socket_fct.h"

// client.c
// usage: ./client [ip address] [port]

int main(int argc, char* argv[])
{
	int peer_sock;

	if (argc != 3)
	{
		printf("usage: ./client [ip address] [port]\n");
		exit(1);
	}

	peer_sock = client_connection(argv[1], argv[2]);

	if (peer_sock == -1)
	{
		close(peer_sock);
		exit(1);
	}
	
	print_socket_info(peer_sock);
	client_communication(peer_sock);

	close(peer_sock);
	exit(0);
}
