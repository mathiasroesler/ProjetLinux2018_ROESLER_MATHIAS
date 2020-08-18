# Linux project
This project was the final project on a course on Linux.

## General information
The objective of the project was to create a client/server messaging system allowing for two people to chat privately. The conversations are not encrypted.

## Features
For the client:
* Name selection when connected
* Exit the conversation by typing the word exit

For the server:
* SIGUSR1 signal closes the server and ends all of the conversations
* SIGUSR2 signal indicates the number of connections to the server

## Launch
To compile the project use the command `make all clean`. This will create two files: client and server.

First launch the server `./server [IP address] [port number] &`. To use the program locally, use 127.0.0.1 as IP address. Otherwise use the IP address of the machine. You can find the IP address by using the command `curl ifconfig.me`. The port number must be greater than 1023.

To connect a client you must launch the client programme using the command `./client [IP address] [port number]`. The IP address and the port number must match the ones of the server. Once two clients are connected they are able to communicate together.

