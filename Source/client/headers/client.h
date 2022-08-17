#pragma once

#include <iostream>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <errno.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <netdb.h>
#include <string.h>
#include <string>

using namespace std;

class Client
{
    int port;
    int sd;
    struct sockaddr_in server;

public:
    Client() = default;
    Client(string, string);
    void sendCommand(string);
    string receiveResponse();
    ~Client();
};