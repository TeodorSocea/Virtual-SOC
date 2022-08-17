#pragma once
#include <string>
#include <vector>
using namespace std;
class Response
{
    string status;
    vector<string> args;

public:
    Response(string, initializer_list<string>);
    Response(string, vector<string>);
    Response(string);
    Response() = default;
    string getStatus();
    vector<string> getArgs();
    string operator*();
};