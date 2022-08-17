#pragma once

#include <string>
#include <vector>
using namespace std;
class Post
{
    int id;
    string title;
    string author;
    string content;
    string date;
    string type;

public:
    Post(int, string, string, string, string, string);
    Post(vector<string>);
    int getId();
    string getHeader();
    string getPost();
    string getTitle();
    string getAuthor();
    string getContent();
    string getType();
    string getDate();
};