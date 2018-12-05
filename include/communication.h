#ifndef COMMUNICATION_H
#define COMMUNICATION_H

#define NAME_SIZE 15
#define BUFFER_SIZE 512

int serverCommunication(int client1_sock, int client2_sock); 
int clientCommunication(int peer_sock);
int serverClientCommunication(int sender_sock, int reader_sock, char sender_buffer[]);
int clientExitHandler(int peer_sock, char reader_buffer[]);
int clientSetup(int peer_sock, char buffer[], char name[]);
int serverSetup(int client1_sock, int client2_sock);
void sigHandlerChild(int sig_num);

#endif /* COMMUNICATION_H */
