#include "../headers/user.h"

User::User(int _id, string _username, string _type)
{
    id = _id;
    username = _username;
    type = _type;
}

int User::getId()
{
    return id;
}

string User::getType()
{
    return type;
}

string User::getUsername()
{
    return username;
}