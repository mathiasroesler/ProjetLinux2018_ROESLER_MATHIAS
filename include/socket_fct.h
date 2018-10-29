#ifndef SOCKET_FCT_H
#define SOCKET_FCT_H

#define NAME_SIZE 15

struct client_info
{
	int socket; 	// Numero du socket 
	int connected;  // Bool indiquant si le client est connecté avec un autre
	char name[NAME_SIZE];  // Nom d'utilisateur
};

typedef struct client_info cli_t;

int streq(char str[], int str_length);
int server_connection(int sock, int nb_connection);
void server_communication(int peer_sock);
int client_connection();
void client_communication(int peer_sock);

#endif /* SOCKET_FCT_H */
