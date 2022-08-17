#pragma once
#include <string>
#include <vector>
using namespace std;
class Command
{
    string id;
    vector<string> args;

public:
    Command(string, initializer_list<string>);
    Command(string, vector<string>);
    Command(string);
    Command() = default;
    string getId();
    vector<string> getArgs();
    string operator*();
};