#include "head.h"

int main()
{	
	WSA_Init();

	sock serverSocket = socketCreation();
	serverSocket = bindServerSocket(serverSocket);
	sock clientSocket = createConnection(serverSocket);
	clientSocket = receiveInformation(clientSocket);
	closeClientSocket(clientSocket);

	return 0;
}