#pragma once
#include <string>

using namespace std;

class User
{
    int id;
    string username;
    string type;

public:
    User(int, string, string);
    string getUsername();
    string getType();
    int getId();
};