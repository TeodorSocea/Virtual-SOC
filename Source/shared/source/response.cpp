#include "../headers/response.h"
#include <sstream>
#include "../headers/base64.h"

Response::Response(string _status, initializer_list<string> _args)
{
    status = _status;
    args = _args;
}

Response::Response(string _status, vector<string> _args)
{
    status = _status;
    args = _args;
}

Response::Response(string packet)
{
    stringstream stream(packet);
    getline(stream, status, ':');
    status = (char *)decode64(status.c_str(), status.length());
    string arg;
    while (getline(stream, arg, ':'))
    {
        string decoded = (char *)decode64(arg.c_str(), arg.length());
        args.push_back(decoded);
    }
}

string Response::getStatus()
{
    return status;
}

vector<string> Response::getArgs()
{
    return args;
}

string Response::operator*()
{
    string output = base64((const unsigned char *)status.c_str(), status.length());
    for (auto &arg : args)
    {
        string encoded = base64((const unsigned char *)arg.c_str(), arg.length());
        output += ":" + encoded;
    }
    return output;
}