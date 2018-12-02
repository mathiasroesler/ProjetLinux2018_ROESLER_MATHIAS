#ifndef INIT_H
#define INIT_H

#define BIND 0
#define CONNECT 1
#define MAX_CONNECTION 4

int createSocket(char* ip, char* port, int flag);
void createServer(int sock);
void sigHandler(int sig_num);

#endif /* INIT_H */
