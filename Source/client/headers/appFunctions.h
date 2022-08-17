#pragma once
#include <string>
#include "../../shared/headers/user.h"
#include "client.h"
using namespace std;
class ClientApp
{
    bool running;
    User *current_user;
    string server_address;
    string port;
    bool check(string);
    string readUsername(string);
    string readPassword(string);
    string readMessage(string);
    User *guestLoginHandle(Client &);
    User *guestRegisterHandle(Client &);
    void guestGetPostsHandle(Client &);
    bool Close(Client &);
    User *authenticatedUserLogoutHandle(Client &);
    void authenticatedUserGetPostsHandle(Client &);
    void authenticatedUserAddFriendHandle(Client &);
    void authenticatedUserCreatePostHandle(Client &);
    void authenticatedUserGetGroupPostsHandle(Client &, string);
    void authenticatedUserGetFriendsHandle(Client &);
    void administratorDeletePostHandle(Client &);
    void authenticatedUserChatWithHandle(Client &, string);

public:
    ClientApp(int, char **);
    void Start();
};

void ReadCin(bool &, bool &, string &);