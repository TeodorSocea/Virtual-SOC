#pragma once
#include <string>
#include <vector>
#include <utility>
#include <tuple>
#include "../../shared/headers/user.h"
#include "../../shared/headers/post.h"
using namespace std;

class DatabaseManager
{

public:
    DatabaseManager();
    bool checkIfUsernameExists(string);
    User *getRegisteredUser(string, string);
    int addNewUserToDatabase(string, string, string);
    int getUserByUsername(string);
    void addFriend(int, int, string);
    vector<Post> getPosts(string);
    vector<Post> getSpecificPosts(string, int);
    vector<string> lookupUsernameNotFriend(int, string);
    vector<string> lookupFriend(int);
    vector<string> getMessages(int, int);
    void addMessage(int, int, string, string);
    void addPost(string, string, string, string);
    void deletePost(int);
};