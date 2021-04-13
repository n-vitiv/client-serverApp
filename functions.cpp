#include "head.h"

//initialization of WSA
void WSA_Init()
{
    //this used only in Windows
    #ifdef _WIN32
    WSADATA wsData;
    WORD ver = MAKEWORD(2,2);

    int wsOK = WSAStartup(ver,&wsData);
    if(wsOK != 0)
    {
        std::cout << "Can`t initialize Windows Socket!" << std::endl;
        exit(1);
    }
    else
    {
        std::cout << "Windows Socket is initialized!" << std::endl;
    }
    #endif
}

//socket creation
sock socketCreation()
{
    sock listening = socket(AF_INET,SOCK_STREAM,IPPROTO_TCP);

    #ifdef _WIN32

    if(listening == INVALID_SOCKET)
    {
        std::cout << "Can`t create a socket. Program stops." << std::endl;
        WSACleanup();
        exit(1);
    }
    
    #elif __linux__

    if(listening == -1)
    {
        std::cout << "Can`t create a socket. Program stops." << std::endl;
        exit(1);
    }

    #endif
    
    return listening;
}

//bind server socket
sock bindServerSocket(sock listening)
{
    sockaddr_in hint;
    hint.sin_addr.s_addr = inet_addr(HOST);
    hint.sin_family = AF_INET;
    hint.sin_port = htons(PORT);

    #ifdef _WIN32

    if(bind(listening, (sockaddr*)&hint, sizeof(hint)) != 0)
    {
        std::cout << "Can`t bind socket. Program stops." << std::endl;
        WSACleanup();
        exit(1);
    }
    listen(listening, SOMAXCONN);

    #elif __linux__

    if(bind(listening, (sockaddr*)&hint, sizeof(hint)) == -1)
    {
        std::cout << "Can`t bind socket. Program stops." << std::endl;
        exit(1);
    }
    if(listen(listening, SOMAXCONN) == -1)
    {
        std::cout << "Can`t listen." << std::endl;
        exit(1);
    }

    #endif

    return listening;
}

//function for closing socket
void closeClientSocket(sock clientSocket)
{
    #ifdef _WIN32
    
    closesocket(clientSocket);
    WSACleanup();
    
    #elif __linux__

    close(clientSocket);

    #endif
}

//function for creating Connection with client
sock createConnection(sock listening)
{
    sockaddr_in client;

    #ifdef _WIN32
    int clientSize = sizeof(client);
    #elif __linux__
    socklen_t clientSize = sizeof(client);
    #endif

    //threads creation for working with multiple client
    std::thread thr[THREAD];
    int threadCount = 0;
    sock clientSocket;

    while(threadCount < THREAD)
    {
        
        clientSocket = accept(listening, (sockaddr*)&client, &clientSize);

        #ifdef _WIN32

        if (clientSocket == INVALID_SOCKET)
        {
            std::cout << "Can`t accept connection. Program stops." << std::endl;
            closesocket(listening);
            WSACleanup();
            return INVALID_SOCKET;
        }
        
        #elif __linux__

        if(clientSocket == -1)
        {
            std::cout << "Can`t accept connection. Program stops." << std::endl;
            close(listening);
            exit(1);
        }

        #endif

        thr[threadCount] = std::thread(receiveInformation, clientSocket);
        threadCount++;
    }

    for(int i = 0; i < THREAD; i++)
    {
        thr[i].join();
    }

    #ifdef _WIN32
    
    closesocket(listening);

    #elif __linux__

    close(listening);
    
    #endif

    return clientSocket;
}

//function for receiving data from client
sock receiveInformation(sock clientSocket)
{
    //buff for information from client
    char buff[BUFF];

    while(true)
    {
        
        int bytesReceived = recv(clientSocket,buff,BUFF,0);

        #ifdef _WIN32
        
        if(bytesReceived == SOCKET_ERROR)
        {
            std::cout << "Error in recv. Stop receiving information." << std::endl;
            break;
        }
        
        #elif __linux__

        if(bytesReceived == -1)
        {
            std::cout << "Error in recv. Stop receiving information." << std::endl;
            break;
        }

        #endif

        if(bytesReceived == 0)
        {
            std::cout << "Client disconnected." << std::endl;
            break;
        }

        printInformation(buff);
    }

    return clientSocket;
}

//print client data into console
void printInformation(char * buff)
{
    std::cout << buff << std::endl;
}

//function for connecting to server
void connectClientSocket(sock clientSocket)
{

    sockaddr_in hint;
    hint.sin_family = AF_INET;
    hint.sin_port = htons(PORT);
    hint.sin_addr.s_addr = inet_addr(HOST);

    int connResult = connect(clientSocket, (sockaddr*)&hint, sizeof(hint));
    
    #ifdef _WIN32

    if(connResult == SOCKET_ERROR)
    {
        std::cout << "Can`t connect to server." << std::endl;
        closesocket(clientSocket);
        WSACleanup();
        exit(1);
    }

    #elif __linux__

    if(connResult == -1)
    {
        std::cout << "Can`t connect to server." << std::endl;
        close(clientSocket);
        exit(1);
    }
    
    #endif

}

//function for send and calculate data
void sendDataToServer(sock clientSocket)
{
    
    std::string buff;
    
    while(true)
    {
        #ifdef _WIN32
        Sleep(PAUSE);
        #elif __linux__
        sleep(PAUSE / 1000);
        #endif

        buff = dataCalculate(buff);

        int sendResult = send(clientSocket, buff.c_str(), BUFF,0);
        
        #ifdef _WIN32

        if(sendResult == SOCKET_ERROR)
        {
            std::cout << "Can`t send data to server." << std::endl;
            break;
        }

        #elif __linux__

        if(sendResult == -1)
        {
            std::cout << "Can`t send data to server." << std::endl;
            break;
        }

        #endif

    }
}

//function for data calculating
std::string dataCalculate(std::string buff)
{
    #ifdef _WIN32

    TCHAR compName[BUFF];
    DWORD compLen = BUFF;
    GetComputerName(compName, &compLen);
    
    buff = (char*)compName;
    buff += " / ";

    //function for obtaining information about the disk space
    DWORD sectorsPerClust, bytesPerSect, freeClust, totalClust;
    GetDiskFreeSpaceA(NULL, &sectorsPerClust, &bytesPerSect, &freeClust, &totalClust);

    //disk memory in bytes
    buff += std::to_string((long long)totalClust * sectorsPerClust * bytesPerSect);
    buff += " / ";
    buff += std::to_string((long long)freeClust * sectorsPerClust * bytesPerSect);

    #elif __linux__

        //function for obtaining information about the computer 
    char compName[BUFF];
    gethostname(compName, BUFF);

    buff = (char*)compName;
    buff += " / ";

    //struct for obtaining information about the disk space
    struct statvfs stat;
    statvfs(get_current_dir_name(), &stat);
        
    //disk memory in bytes
    buff += std::to_string((long long)stat.f_bsize * stat.f_blocks);
    buff += " / ";
    buff += std::to_string((long long)stat.f_bsize * stat.f_bavail);

    #endif

    return buff;
}