#pragma once

#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <errno.h>
#include <unistd.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <wait.h>
#include <string>

using namespace std;
void sighandle(int);
class Server
{
    struct sockaddr_in server;
    struct sockaddr_in from;
    int sd;
    int port;

public:
    Server();
    int SAccept();
    void sendResponse(int, string);
    string receiveCommand(int);
    ~Server();
};