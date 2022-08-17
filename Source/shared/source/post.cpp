#include "../headers/post.h"
#include <iostream>
#include <time.h>
Post::Post(int _id, string _title, string _author, string _content, string _type, string _date)
{
    id = _id;
    title = _title;
    author = _author;
    content = _content;
    type = _type;
    date = _date;
}

Post::Post(vector<string> args)
{
    id = stoi(args[0]);
    title = args[1];
    author = args[2];
    content = args[3];
    type = args[4];
    time_t time = stoi(args[5]);
    date = asctime(localtime(&time));
    date.pop_back();
}

int Post::getId()
{
    return id;
}

string Post::getTitle()
{
    return title;
}

string Post::getAuthor()
{
    return author;
}

string Post::getContent()
{
    return content;
}

string Post::getType()
{
    return type;
}
string Post::getDate()
{
    return date;
}

string Post::getHeader()
{
    return "\"" + title + "\"" + " by " + author + " on " + date;
}

string Post::getPost()
{
    return "Title: " + title + "\nPosted by: " + author + " on " + date + "\n" + content;
}
