#ifndef _HEAD_H_
#define _HEAD_H_

#define PORT 5555           //port number
#define PAUSE 5000          //ms between requests
#define HOST "127.0.0.1"    //local host
#define BUFF 128            //the size of the buffer to receive the message
#define THREAD 4            //Max threads number on which server is running

//platform-independent libraries
#include <iostream>
#include <string>
#include <thread>

#ifdef _WIN32

#include <WS2tcpip.h>
#pragma comment (lib, "ws2_32.lib")
typedef SOCKET sock;

#elif __linux__

#include <sys/types.h>
#include <unistd.h>
#include <sys/socket.h>
#include <netdb.h>
#include <arpa/inet.h>
#include <sys/statvfs.h>
typedef int sock;

#else
#error Run on not supported OS
#endif

//functions for initializing and creating a socket
void WSA_Init();
sock socketCreation();

//server functions

//function for binding server socket
sock bindServerSocket(sock listening);

//function for get connection
sock createConnection(sock listening);

//function for receiving info
sock receiveInformation(sock clientSocket);

//function for printing info into server console
void printInformation(char * buff);

//close socket
void closeClientSocket(sock clientSocket);

//client functions

//connect function
void connectClientSocket(sock clientSocket);

//function for sending data to server
void sendDataToServer(sock clientSocket);

//function for data calculating
std::string dataCalculate(std::string buff);

#endif