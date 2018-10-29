#ifndef SOCKET_FCT_H
#define SOCKET_FCT_H

int create_socket(char *ip, char *address);
int print_socket_info(int sock);
int print_peer_info(int sock);
int streq(char str[], int str_length);
int server_connection(int sock, int nb_connection);
void server_communication(int peer_sock);
int client_connection();
void client_communication(int peer_sock);

#endif /* SOCKET_FCT_H */
