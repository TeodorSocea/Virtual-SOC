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
#include "../headers/client.h"
#include <string>

using namespace std;

Client::Client(string server_address, string _port)
{
    port = atoi(_port.c_str());

    if ((sd = socket(AF_INET, SOCK_STREAM, 0)) == -1)
    {
        perror("Error at socket().\n");
        exit(-1);
    }

    server.sin_family = AF_INET;
    server.sin_addr.s_addr = inet_addr(server_address.c_str());
    server.sin_port = htons(port);

    if (connect(sd, (struct sockaddr *)&server, sizeof(struct sockaddr)) == -1)
    {
        perror("[client]Error at connect().\n");
        exit(-1);
    }
}

void Client::sendCommand(string command)
{
    int size = command.length();
    if (write(sd, &size, 4) <= 0)
    {
        perror("[client]Error at write() size to server.\n");
        exit(-1);
    }
    if (write(sd, command.c_str(), command.length()) <= 0)
    {
        perror("[client]Error at write() command to server.\n");
        exit(-1);
    }
}

string Client::receiveResponse()
{
    int size;
    if (read(sd, &size, 4) <= 0)
    {
        perror("[client]Error at read() size from server.\n");
        exit(-1);
    }

    char *response = new char[size];

    if (read(sd, response, size) <= 0)
    {
        perror("[client]Error at read() response from server.\n");
        exit(-1);
    }
    string output = "";
    for (int i = 0; i < size; i++)
    {
        output += response[i];
    }
    delete response;
    return output;
}

Client::~Client()
{
    close(sd);
}