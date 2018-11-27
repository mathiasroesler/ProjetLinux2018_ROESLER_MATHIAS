#ifndef INIT_H
#define INIT_H

#define BIND 0
#define CONNECT 1

int createSocket(char* ip, char* port, int flag);
int createShm();
void initServer(int sock);

#endif /* INIT_H */
