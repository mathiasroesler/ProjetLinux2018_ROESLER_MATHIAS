#ifndef SOCKET_FCT_H
#define SOCKET_FCT_H

#define NAME_SIZE 15

struct clientInfo
{
	int socket; 	// Numero du socket 
	int connected;  // Bool indiquant si le client est connecté avec un autre
	char name[NAME_SIZE];  // Nom d'utilisateur
};

typedef struct clientInfo cli_t;

int streq(char str[], int str_length);
void serverCommunication(int client1_sock, int client2_sock); 
int clientConnection();
void clientCommunication(int peer_sock);

#endif /* SOCKET_FCT_H */
