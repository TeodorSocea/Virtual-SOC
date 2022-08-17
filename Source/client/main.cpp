#include "headers/appFunctions.h"
#include "../shared/headers/user.h"
#include "headers/client.h"
#include <iostream>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <netdb.h>
#include <string.h>

int main(int argc, char *argv[])
{
    if (argc != 3)
    {
        cout << "Usage: ./ClientApp <server-ip> <server-port>" << endl;
        return 0;
    }
    ClientApp app(argc, argv);
    app.Start();
    return 0;
}