#include "../headers/command.h"
#include <sstream>
#include "../headers/base64.h"

Command::Command(string _id, initializer_list<string> _args)
{
    id = _id;
    args = _args;
}

Command::Command(string _id, vector<string> _args)
{
    id = _id;
    args = _args;
}

Command::Command(string packet)
{
    stringstream stream(packet);
    getline(stream, id, ':');
    id = (char *)decode64(id.c_str(), id.length());
    string arg;
    while (getline(stream, arg, ':'))
    {
        string decoded = (char *)decode64(arg.c_str(), arg.length());
        args.push_back(decoded);
    }
}

string Command::getId()
{
    return id;
}

vector<string> Command::getArgs()
{
    return args;
}

string Command::operator*()
{
    string output = base64((const unsigned char *)id.c_str(), id.length());
    for (auto &arg : args)
    {
        string encoded = base64((const unsigned char *)arg.c_str(), arg.length());
        output += ":" + encoded;
    }
    return output;
}