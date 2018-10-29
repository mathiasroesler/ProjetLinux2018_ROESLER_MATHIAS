#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include "socket_fct.h"

int main(int argc, char* argv[])
{
	int sock;
	int peer_sock;
	
	if (argc != 3)
	{
		printf("usage: ./server [ip address] [port]\n");
		exit(1);
	}
	
	sock = create_socket(argv[1], argv[2]);

	if (sock == -1)
	{
		close(sock);
		exit(1);
	}

	print_socket_info(sock);

	peer_sock = server_connection(sock, 2);

	close(sock);
	close(peer_sock);
	exit(0);

}
