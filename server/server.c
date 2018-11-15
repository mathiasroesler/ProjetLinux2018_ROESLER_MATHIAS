#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include "../include/socket_fct.h"
#include "../include/misc.h"
#include "../include/init.h"

// server.c
// usage: ./server [ip address] [port]

int main(int argc, char* argv[])
{
	int sock;
	
	if (argc != 3)
	{
		printf("usage: ./server [ip address] [port]\n");
		exit(1);
	}
	
	sock = create_socket(argv[1], argv[2], BIND);

	if (sock == -1)
	{
		close(sock);
		exit(1);
	}

	print_socket_info(sock);

	init_server(sock, 5); 

	close(sock);
	exit(0);

}
