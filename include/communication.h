#ifndef COMMUNICATION_H
#define COMMUNICATION_H

#define NAME_SIZE 15
#define BUFFER_SIZE 512

int serverCommunication(int client1_sock, int client2_sock); 
int clientCommunication(int peer_sock);
void sigHandlerChild(int sig_num);

#endif /* COMMUNICATION_H */
