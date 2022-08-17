#include "../headers/server.h"
#include <sys/ioctl.h>
#include <iostream>
void sighandle(int sig)
{
    while (0 < waitpid(-1, NULL, WNOHANG))
    {
    }
}

Server::Server()
{
    port = 2030;
    if ((sd = socket(AF_INET, SOCK_STREAM, 0)) == -1)
    {
        perror("[server]Error at socket().\n");
        exit(-1);
    }
    bzero(&server, sizeof(server));
    bzero(&from, sizeof(from));

    server.sin_family = AF_INET;
    server.sin_addr.s_addr = htonl(INADDR_ANY);
    server.sin_port = htons(port);

    if (bind(sd, (struct sockaddr *)&server, sizeof(struct sockaddr)) == -1)
    {
        perror("[server]Error at bind().\n");
        exit(-1);
    }

    if (listen(sd, 5) == -1)
    {
        perror("[server]Error at listen().\n");
        exit(-1);
    }
}

int Server::SAccept()
{
    socklen_t length = sizeof(from);
    int p = accept(sd, (struct sockaddr *)&from, &length);
    return p;
}

string Server::receiveCommand(int client)
{
    int size;
    if (read(client, &size, 4) <= 0)
    {
        perror("[server]Error at read() size from client.\n");
        close(client);
        exit(-1);
    }

    char *command = new char[size];

    if (read(client, command, size) <= 0)
    {
        perror("[server]Error at read() command from client.\n");
        close(client);
        exit(-1);
    }
    string output = "";
    for (int i = 0; i < size; i++)
    {
        output += command[i];
    }
    delete command;
    return output;
}

void Server::sendResponse(int client, string response)
{
    int size = response.length();
    if (write(client, &size, 4) <= 0)
    {
        perror("[server]Error at write() size to client.\n");
        exit(-1);
    }
    if (write(client, response.c_str(), size) <= 0)
    {
        perror("[server]Error at write() response to client.\n");
        exit(-1);
    }
}

Server::~Server()
{
    close(sd);
}