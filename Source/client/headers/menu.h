#pragma once

#include "selector.h"
#include <vector>
#include <string>
using namespace std;

char getch(void);
class Menu
{
    string header;
    vector<string> content;
    Selector selector;

public:
    Menu(string, vector<string> &);
    int print();
};