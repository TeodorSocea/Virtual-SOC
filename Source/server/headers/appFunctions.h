#include "server.h"
#include "../../shared/headers/command.h"
#include "../../shared/headers/user.h"
#include "../headers/databaseManager.h"
#include <string>
using namespace std;
class ServerApp
{
    bool running;
    Server server;
    DatabaseManager database;
    User *current_user;
    User *guestLoginHandle(vector<string>);
    User *guestRegisterHandle(vector<string>);
    void guestGetPostsHandle(int);
    User *authenticatedUserLogoutHandle();
    void authenticatedUserAddFriendHandle(int, string);
    void authenticatedUserCreatePost(int, string, string, string);
    void authenticatedUserGetPostsHandle(int, string);
    void authenticatedUserGetFriendsHandle(int);
    void administratorDeletePostHandle(int);
    void authenticatedUserChatWithHandle(int, int);

public:
    ServerApp();
    void Start();
};