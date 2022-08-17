#include "../headers/appFunctions.h"
#include "../../shared/headers/response.h"
#include <iostream>
#include <sstream>
#include <algorithm>
ServerApp::ServerApp()
{
    running = true;
    current_user = new User(-1, "Guest", "GUEST");
}

User *ServerApp::guestRegisterHandle(vector<string> args)
{
    string username = args[0];
    string password = args[1];
    string type = "REGULAR";
    if (database.checkIfUsernameExists(username) == true)
        return nullptr;
    int id = database.addNewUserToDatabase(username, password, type);
    return new User(id, username, "REGULAR");
}

User *ServerApp::guestLoginHandle(vector<string> args)
{
    string username = args[0];
    string password = args[1];
    return database.getRegisteredUser(username, password);
}

void ServerApp::guestGetPostsHandle(int client)
{
    vector<Post> posts = database.getPosts("PUBLIC");
    while (true)
    {
        int size = posts.size();
        if (size == 0)
        {
            Response response("NOPOSTS", {});
            server.sendResponse(client, *response);
            break;
        }
        else
        {
            Response response("SUCCESS", {to_string(min(10, size))});
            server.sendResponse(client, *response);
            for (int i = 0; i < min(10, size); i++)
            {
                Post to_send = posts.back();
                Response post_response("POST", {to_string(to_send.getId()),
                                                to_send.getTitle(),
                                                to_send.getAuthor(),
                                                to_send.getContent(),
                                                to_send.getType(),
                                                to_send.getDate()});
                posts.pop_back();
                server.sendResponse(client, *post_response);
            }
            Command command(server.receiveCommand(client));
            if (command.getId() == "DONE")
                break;
        }
    }
}

void ServerApp::administratorDeletePostHandle(int client)
{
    vector<Post> posts = database.getPosts("ALL");
    while (true)
    {
        int size = posts.size();
        if (size == 0)
        {
            Response response("NOPOSTS", {});
            server.sendResponse(client, *response);
            break;
        }
        else
        {
            Response response("SUCCESS", {to_string(min(10, size))});
            server.sendResponse(client, *response);
            for (int i = 0; i < min(10, size); i++)
            {
                Post to_send = posts.back();
                Response post_response("POST", {to_string(to_send.getId()),
                                                to_send.getTitle(),
                                                to_send.getAuthor(),
                                                to_send.getContent(),
                                                to_send.getType(),
                                                to_send.getDate()});
                posts.pop_back();
                server.sendResponse(client, *post_response);
            }
            Command command(server.receiveCommand(client));
            if (command.getId() == "DONE")
                return;
            else if (command.getId() == "MORE")
                continue;
            else if (command.getId() == "DELETE")
            {
                database.deletePost(stoi(command.getArgs()[0]));
                Response delete_response("SUCCESS", {});
                server.sendResponse(client, *delete_response);
                return;
            }
        }
    }
}

void ServerApp::authenticatedUserGetPostsHandle(int client, string type)
{
    vector<Post> posts = database.getSpecificPosts(type, current_user->getId());
    while (true)
    {
        int size = posts.size();
        if (size == 0)
        {
            Response response("NOPOSTS", {});
            server.sendResponse(client, *response);
            break;
        }
        else
        {
            Response response("SUCCESS", {to_string(min(10, size))});
            server.sendResponse(client, *response);
            for (int i = 0; i < min(10, size); i++)
            {
                Post to_send = posts.back();
                Response post_response("POST", {to_string(to_send.getId()),
                                                to_send.getTitle(),
                                                to_send.getAuthor(),
                                                to_send.getContent(),
                                                to_send.getType(),
                                                to_send.getDate()});
                posts.pop_back();
                server.sendResponse(client, *post_response);
            }
            Command command(server.receiveCommand(client));
            if (command.getId() == "DONE")
                break;
        }
    }
}

User *ServerApp::authenticatedUserLogoutHandle()
{
    return new User(-1, "Guest", "GUEST");
}

void ServerApp::authenticatedUserAddFriendHandle(int client, string username)
{
    vector<string> result = database.lookupUsernameNotFriend(current_user->getId(), username);
    while (true)
    {
        int size = result.size();
        if (size == 0)
        {
            Response response("NOFRIENDS", {});
            server.sendResponse(client, *response);
            break;
        }
        else
        {
            Response response("SUCCESS", {to_string(min(10, size))});
            server.sendResponse(client, *response);
            for (int i = 0; i < min(10, size); i++)
            {
                string to_send = result.back();
                Response friend_response("FRIEND", {to_send});
                result.pop_back();
                server.sendResponse(client, *friend_response);
            }
            Command command(server.receiveCommand(client));
            if (command.getId() == "DONE")
                break;
            else if (command.getId() == "ADDFRIEND")
            {
                int friend_id = database.getUserByUsername(command.getArgs()[0]);
                database.addFriend(current_user->getId(), friend_id, command.getArgs()[1]);
                Response friend_response("SUCCESS", {});
                server.sendResponse(client, *friend_response);
                break;
            }
            else if (command.getId() == "MORE")
            {
                continue;
            }
        }
    }
}

void ServerApp::authenticatedUserGetFriendsHandle(int client)
{
    vector<string> result = database.lookupFriend(current_user->getId());
    while (true)
    {
        int size = result.size();
        if (size == 0)
        {
            Response response("NOFRIENDS", {});
            server.sendResponse(client, *response);
            break;
        }
        else
        {
            Response response("SUCCESS", {to_string(min(10, size))});
            server.sendResponse(client, *response);
            for (int i = 0; i < min(10, size); i++)
            {
                string to_send = result.back();
                Response friend_response("FRIEND", {to_send});
                result.pop_back();
                server.sendResponse(client, *friend_response);
            }
            Command command(server.receiveCommand(client));
            if (command.getId() == "DONE")
                break;
            else if (command.getId() == "MORE")
            {
                continue;
            }
            else if (command.getId() == "CHATWITH")
            {
                int friendid = database.getUserByUsername(command.getArgs()[0]);
                authenticatedUserChatWithHandle(client, friendid);
                return;
            }
        }
    }
}

void ServerApp::authenticatedUserChatWithHandle(int client, int friend_id)
{
    while (true)
    {
        vector<string> messages = database.getMessages(current_user->getId(), friend_id);
        int size = messages.size();
        cout << size << endl;
        Response response("SUCCESS", {to_string(min(size, 10))});
        server.sendResponse(client, *response);
        for (int i = max(0, size - 10); i < size; i++)
        {
            string to_send = messages[i];
            Response message_response("MESSAGE", {to_send});
            server.sendResponse(client, *message_response);
        }
        cout << "Sent messages\n";
        Command command(server.receiveCommand(client));
        if (command.getId() == "DONE")
            return;
        else if (command.getId() == "MESSAGE")
        {
            database.addMessage(current_user->getId(), friend_id, current_user->getUsername(), command.getArgs()[0]);
        }
        else if (command.getId() == "REFRESH")
        {
            continue;
        }
    }
}

void ServerApp::authenticatedUserCreatePost(int client, string title, string content, string type)
{
    database.addPost(title, current_user->getUsername(), content, type);
    Response response("SUCCESS", {});
    server.sendResponse(client, *response);
}

void ServerApp::Start()
{
    if (SIG_ERR == signal(SIGCHLD, sighandle))
    {
        perror("Signal()");
        exit(-1);
    }
    int client;
    while (running)
    {
        cout << "Waiting for client" << endl;
        client = server.SAccept();
        if (client < 0)
        {
            perror("[server]Error at accept().\n");
            exit(-1);
        }
        cout << "A client was accepted, passing it over to a child process." << endl;
        int pid = fork();
        if (pid == 0)
        {
            cout << "Entered child process." << endl;
            Command command;
            do
            {
                command = Command(server.receiveCommand(client));
                cout << *command << endl;
                if (command.getId() == "LOGIN")
                {
                    if (database.checkIfUsernameExists(command.getArgs()[0]) == false)
                    {
                        Response response("FAILURE", {"Account does not exist!"});
                        server.sendResponse(client, *response);
                    }
                    else
                    {
                        User *responseUser = guestLoginHandle(command.getArgs());
                        if (responseUser != nullptr)
                        {
                            delete current_user;
                            current_user = responseUser;
                            Response response("SUCCESS", {to_string(current_user->getId()), current_user->getUsername(), current_user->getType()});
                            server.sendResponse(client, *response);
                        }
                        else
                        {
                            Response response("FAILURE", {"Incorrect username or password!"});
                            server.sendResponse(client, *response);
                        }
                    }
                }
                else if (command.getId() == "REGISTER")
                {
                    User *responseUser = guestRegisterHandle(command.getArgs());
                    if (responseUser != nullptr)
                    {
                        delete current_user;
                        current_user = responseUser;
                        Response response("SUCCESS", {to_string(current_user->getId()), current_user->getUsername(), current_user->getType()});
                        server.sendResponse(client, *response);
                    }
                    else
                    {
                        Response response("FAILURE", {"Username already exists!"});
                        server.sendResponse(client, *response);
                    }
                }
                else if (command.getId() == "GETPOSTS")
                {
                    if (command.getArgs()[0] == "PUBLIC")
                    {
                        guestGetPostsHandle(client);
                    }
                    else if (command.getArgs()[0] == "ADMIN")
                    {
                        administratorDeletePostHandle(client);
                    }
                    else
                    {
                        authenticatedUserGetPostsHandle(client, command.getArgs()[0]);
                    }
                }
                else if (command.getId() == "LOGOUT")
                {
                    delete current_user;
                    current_user = authenticatedUserLogoutHandle();
                    Response response("SUCCESS", {to_string(current_user->getId()), current_user->getUsername(), current_user->getType()});
                    server.sendResponse(client, *response);
                }
                else if (command.getId() == "LOOKUP")
                {
                    if (command.getArgs()[0] == "ALL")
                    {
                        authenticatedUserAddFriendHandle(client, "");
                    }
                    else if (command.getArgs()[0] == "FRIENDS")
                    {
                        authenticatedUserGetFriendsHandle(client);
                    }
                    else
                    {
                        authenticatedUserAddFriendHandle(client, command.getArgs()[0]);
                    }
                }
                else if (command.getId() == "POSTCREATE")
                {
                    authenticatedUserCreatePost(client, command.getArgs()[0], command.getArgs()[1], command.getArgs()[2]);
                }
                else if (command.getId() != "QUITSESSION")
                {
                    continue;
                }

            } while (command.getId() != "QUITSESSION");
            cout << "Exiting out of child" << endl;
            close(client);
            exit(SIGCHLD);
        }
        else
        {
            close(client);
            continue;
        }
    }
}