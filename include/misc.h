#ifndef MISC_H
#define MISC_H

#include "socket_fct.h"

int print_socket_info(int sock);
int print_peer_info(int peer_sock);
int streq(char str[], int str_length);
void fill_client_info(cli_t cli, int sock, int connected, char name[NAME_SIZE]);
void init_shm(cli_t cli[]);
void print_client_info(cli_t cli);

#endif /* MISH_H */
