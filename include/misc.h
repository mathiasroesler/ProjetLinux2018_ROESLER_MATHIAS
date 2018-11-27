#ifndef MISC_H
#define MISC_H

#include "socket_fct.h"

int printSocketInfo(int sock);
int printPeerInfo(int peer_sock);
int streq(char str[], int str_length);
void fillClientInfo(cli_t cli, int sock, int connected, char name[NAME_SIZE]);
void printClientInfo(cli_t cli);
int nbConnection(int *pid_mem, int max_connection);
void strCat(char name[], char buffer[], char message[]);
void setNonBlocking(int sock);

#endif /* MISH_H */
