#include "head.h"

int main()
{
    WSA_Init();

    sock client = socketCreation();
    connectClientSocket(client);
    sendDataToServer(client);

    closeClientSocket(client);

    return 0;
}