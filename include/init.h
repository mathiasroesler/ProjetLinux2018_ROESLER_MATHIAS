#ifndef INIT_H
#define INIT_H

#define BIND 0
#define CONNECT 1

int create_socket(char* ip, char* port, int flag);
int create_shm();
void init_server(int sock, int nb_connection);

#endif /* INIT_H */
