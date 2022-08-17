#include "../headers/appFunctions.h"
#include "../headers/menu.h"
#include "../headers/menus.h"
#include "../../shared/headers/command.h"
#include "../../shared/headers/response.h"
#include "../../shared/headers/post.h"
#include <iostream>
#include <sstream>
#include <vector>
#include <iomanip>
#include <openssl/evp.h>
#include <openssl/sha.h>
#include <unistd.h>
#include <thread>
//Credit to user aurius on Stack Overflow for sha3_512 and bytes_to_hex_string functions
std::string bytes_to_hex_string(const std::vector<uint8_t> &bytes)
{
    std::ostringstream stream;
    for (uint8_t b : bytes)
    {
        stream << std::setw(2) << std::setfill('0') << std::hex << static_cast<int>(b);
    }
    return stream.str();
}

std::string sha3_512(const std::string &input)
{
    uint32_t digest_length = SHA512_DIGEST_LENGTH;
    const EVP_MD *algorithm = EVP_sha3_512();
    uint8_t *digest = static_cast<uint8_t *>(OPENSSL_malloc(digest_length));
    EVP_MD_CTX *context = EVP_MD_CTX_new();
    EVP_DigestInit_ex(context, algorithm, nullptr);
    EVP_DigestUpdate(context, input.c_str(), input.size());
    EVP_DigestFinal_ex(context, digest, &digest_length);
    EVP_MD_CTX_destroy(context);
    std::string output = bytes_to_hex_string(std::vector<uint8_t>(digest, digest + digest_length));
    OPENSSL_free(digest);
    return output;
}

ClientApp::ClientApp(int argc, char *argv[])
{
    running = true;
    current_user = new User(-1, "Guest", "GUEST");
    server_address = argv[1];
    port = argv[2];
}

bool ClientApp::check(string input)
{
    if (input.length() < 6 || input.length() > 19)
        return false;
    string banned = "!@#$%^&*()=+[]{}|/,.<>;:\'\"`";
    for (auto &b : banned)
    {
        if (input.find(b) != std::string::npos)
            return false;
    }
    return true;
}

string ClientApp::readUsername(string message)
{
    system("clear");
    string output = "";
    cout << message;
    char keypress;
    while ((keypress = getch()) != 10)
    {
        if (keypress == 27)
            return "ESC";
        else if (keypress == 127)
        {
            if (!output.empty())
            {
                output.pop_back();
                cout << "\b \b";
            }
        }
        else
        {
            output += keypress;
            cout << keypress;
        }
    }
    return output;
}

string ClientApp::readMessage(string message)
{
    string output = "";
    cout << message;
    char keypress;
    while ((keypress = getch()) != 10)
    {
        if (keypress == 27)
            return "ESC";
        else if (keypress == 127)
        {
            if (!output.empty())
            {
                output.pop_back();
                cout << "\b \b";
            }
        }
        else
        {
            output += keypress;
            cout << keypress;
        }
    }
    return output;
}

string ClientApp::readPassword(string message)
{
    system("clear");
    string output = "";
    cout << message;
    char keypress;
    while ((keypress = getch()) != 10)
    {
        if (keypress == 27)
            return "ESC";
        else if (keypress == 127)
        {
            if (!output.empty())
            {
                output.pop_back();
                cout << "\b \b";
            }
        }
        else
        {
            output += keypress;
            cout << '*';
        }
    }
    return output;
}

User *ClientApp::guestLoginHandle(Client &client)
{
    string username;
    while (true)
    {
        if ((username = readUsername("Press \"ESC\" to exit.\nEnter your username: ")) == "ESC")
            return nullptr;
        if (check(username))
            break;
        system("clear");
        cout << "Username too short/long (min 6/max 20) or contains illegal characters (!@#$%^&*()=+[]{}|/,.<>;:\'\"`)\n";
        cout << "Hit \"Enter\" to continue.";
        cin.ignore();
    }
    string password;
    while (true)
    {
        if ((password = readPassword("Press \"ESC\" to exit.\nEnter your password: ")) == "ESC")
            return nullptr;
        if (check(password))
            break;
        system("clear");
        cout << "Password too short/long (min 6/max 20) or contains illegal characters (!@#$%^&*()=+[]{}|/,.<>;:\'\"`)\n";
        cout << "Hit \"Enter\" to continue.";
        cin.ignore();
    }
    password = sha3_512(password);
    Command command("LOGIN", {username, password});
    client.sendCommand(*command);
    Response response(client.receiveResponse());
    if (response.getStatus() == "SUCCESS")
    {
        system("clear");
        cout << "Successfully logged in! Hit \"Enter\" to continue to your account!";
        cin.ignore();
        return new User(stoi(response.getArgs()[0]), response.getArgs()[1], response.getArgs()[2]);
    }
    else
    {
        system("clear");
        cout << response.getStatus() << ": " << response.getArgs()[0] << " Hit \"Enter\" to continue!";
        cin.ignore();
        return nullptr;
    }
}

User *ClientApp::
    guestRegisterHandle(Client &client)
{
    string username, password, repeatPassword;
    while (true)
    {
        if ("ESC" == (username = readUsername("Press \"ESC\" to exit.\nEnter your username: ")))
            return nullptr;
        if (check(username))
            break;
        system("clear");
        cout << "Username too short/long (min 6/max 20) or contains illegal characters (!@#$%^&*()=+[]{}|/,.<>;:\'\"`)\n";
        cout << "Hit \"Enter\" to continue.";
        cin.ignore();
    }
    while (true)
    {
        if ("ESC" == (password = readPassword("Press \"ESC\" to exit.\nEnter your password: ")))
            return nullptr;
        if ("ESC" == (repeatPassword = readPassword("Press \"ESC\" to exit.\nRe-enter your password: ")))
            return nullptr;
        if (password == repeatPassword)
        {
            if (check(password))
                break;
            system("clear");
            cout << "Password too short/long (min 6/max 20) or contains illegal characters (!@#$%^&*()=+[]{}|/,.<>;:\'\"`)\n";
            cout << "Hit \"Enter\" to continue.";
            cin.ignore();
        }
        else
        {
            system("clear");
            cout << "Password does not match! Hit \"Enter\" to try again!" << endl;
            cin.ignore();
        }
    }
    password = repeatPassword = sha3_512(password);
    Command command("REGISTER", {username, password});
    client.sendCommand(*command);
    Response response(client.receiveResponse());
    if (response.getStatus() == "SUCCESS")
    {
        system("clear");
        cout << "Account registration successfull! Hit \"Enter\" to continue to your account!";
        cin.ignore();
        return new User(stoi(response.getArgs()[0]), response.getArgs()[1], response.getArgs()[2]);
    }
    else
    {
        system("clear");
        cout << response.getStatus() << ": " << response.getArgs()[0] << " Hit \"Enter\" to continue!";
        cin.ignore();
        return nullptr;
    }
}

void ClientApp::guestGetPostsHandle(Client &client)
{
    Command command("GETPOSTS", {"PUBLIC"});
    client.sendCommand(*command);

    bool more = true;
    while (more)
    {
        Response response(client.receiveResponse());
        if (response.getStatus() == "SUCCESS")
        {
            int posts_number = stoi(response.getArgs()[0]);
            vector<Post> posts;
            vector<string> posts_headers;
            for (int i = 0; i < posts_number; i++)
            {
                string smth = client.receiveResponse();
                Response post_response(smth);
                posts.push_back(post_response.getArgs());
                posts_headers.push_back(posts.back().getHeader());
            }
            posts_headers.push_back("More posts");
            posts_headers.push_back("Exit");
            int exit_id = posts_headers.size() - 1;
            int more_id = exit_id - 1;
            while (true)
            {
                Menu posts_menu("These are the latest posts:", posts_headers);
                int user_choice = posts_menu.print();
                if (user_choice == exit_id)
                {
                    Command command("DONE", {});
                    client.sendCommand(*command);
                    more = false;
                    break;
                }
                else if (user_choice == more_id)
                {
                    Command command("MORE", {});
                    client.sendCommand(*command);
                    break;
                }
                else
                {
                    system("clear");
                    cout << posts[user_choice].getPost();
                    cout << "\n\nHit \"Enter\" to continue.";
                    while (getch() != 10)
                    {
                    }
                }
            }
        }
        else if (response.getStatus() == "NOPOSTS")
        {
            cout << "No more posts available! Hit \"Enter\" to continue.";
            cin.ignore();
            more = false;
        }
    }
}

void ClientApp::administratorDeletePostHandle(Client &client)
{
    Command command("GETPOSTS", {"ADMIN"});
    client.sendCommand(*command);

    bool more = true;
    while (more)
    {
        Response response(client.receiveResponse());
        if (response.getStatus() == "SUCCESS")
        {
            int posts_number = stoi(response.getArgs()[0]);
            vector<Post> posts;
            vector<string> posts_headers;
            for (int i = 0; i < posts_number; i++)
            {
                string smth = client.receiveResponse();
                Response post_response(smth);
                posts.push_back(post_response.getArgs());
                posts_headers.push_back(posts.back().getHeader());
            }
            posts_headers.push_back("More posts");
            posts_headers.push_back("Exit");
            int exit_id = posts_headers.size() - 1;
            int more_id = exit_id - 1;
            bool viewing = true;
            while (viewing)
            {
                Menu posts_menu("These are the latest posts:", posts_headers);
                int user_choice = posts_menu.print();
                if (user_choice == exit_id)
                {
                    Command command("DONE", {});
                    client.sendCommand(*command);
                    more = false;
                    break;
                }
                else if (user_choice == more_id)
                {
                    Command command("MORE", {});
                    client.sendCommand(*command);
                    break;
                }
                else
                {
                    system("clear");
                    cout << posts[user_choice].getPost();
                    cout << "\n\nHit \"Enter\" to continue.";
                    cout << "\nHit \"X\" to delete the post.";
                    char c = getch();
                    while (c != 10)
                    {
                        bool stop = false;
                        if (c == 'x' || c == 'X')
                        {
                            system("clear");
                            Menu confirm_menu("Are you sure you want to delete this post?", confirm_options);
                            int choice = confirm_menu.print();
                            switch (choice)
                            {
                            case 0:
                            {
                                stop = true;
                                Command command_delete("DELETE", {to_string(posts[user_choice].getId())});
                                client.sendCommand(*command_delete);
                                Response response_delete(client.receiveResponse());
                                if (response_delete.getStatus() == "SUCCESS")
                                {
                                    cout << "Post deleted successfully!\nHit \"Enter\" to continue.";
                                    while (getch() != 10)
                                    {
                                    }
                                    return;
                                }
                                else
                                {
                                    cout << "Post was not deleted!\nHit \"Enter\" to continue.";
                                    while (getch() != 10)
                                    {
                                    }
                                }
                                break;
                            }
                            case 1:
                            {
                                cout << "Post was not deleted!\nHit \"Enter\" to continue.";
                                cin.ignore();
                                stop = true;
                                break;
                            }
                            }
                        }
                        if (stop)
                            break;
                        c = getch();
                    }
                }
            }
        }
        else if (response.getStatus() == "NOPOSTS")
        {
            cout << "No more posts available! Hit \"Enter\" to continue.";
            cin.ignore();
            more = false;
        }
    }
}

bool ClientApp::Close(Client &client)
{
    Command command("QUITSESSION", {});
    client.sendCommand(*command);
    return false;
}

User *ClientApp::authenticatedUserLogoutHandle(Client &client)
{
    Command command("LOGOUT", {});
    client.sendCommand(*command);
    Response response(client.receiveResponse());
    if (response.getStatus() == "SUCCESS")
    {
        cout << "Successfully logged out!\nHit \"Enter\" to continue.";
        cin.ignore();
        return new User(stoi(response.getArgs()[0]), response.getArgs()[1], response.getArgs()[2]);
    }
    else
    {
        return nullptr;
    }
}

void ClientApp::authenticatedUserGetGroupPostsHandle(Client &client, string type)
{
    Command command("GETPOSTS", {type});
    client.sendCommand(*command);

    bool more = true;
    while (more)
    {
        Response response(client.receiveResponse());
        if (response.getStatus() == "SUCCESS")
        {
            int posts_number = stoi(response.getArgs()[0]);
            vector<Post> posts;
            vector<string> posts_headers;
            for (int i = 0; i < posts_number; i++)
            {
                string smth = client.receiveResponse();
                Response post_response(smth);
                posts.push_back(post_response.getArgs());
                posts_headers.push_back(posts.back().getHeader());
            }
            posts_headers.push_back("More posts");
            posts_headers.push_back("Exit");
            int exit_id = posts_headers.size() - 1;
            int more_id = exit_id - 1;
            while (true)
            {
                Menu posts_menu("These are the latest posts:", posts_headers);
                int user_choice = posts_menu.print();
                if (user_choice == exit_id)
                {
                    Command command("DONE", {});
                    client.sendCommand(*command);
                    more = false;
                    break;
                }
                else if (user_choice == more_id)
                {
                    Command command("MORE", {});
                    client.sendCommand(*command);
                    break;
                }
                else
                {
                    system("clear");
                    cout << posts[user_choice].getPost();
                    cout << "\n\nHit \"Enter\" to continue.";
                    while (getch() != 10)
                    {
                    }
                }
            }
        }
        else if (response.getStatus() == "NOPOSTS")
        {
            cout << "No more posts available! Hit \"Enter\" to continue.";
            cin.ignore();
            more = false;
        }
    }
}

void ClientApp::authenticatedUserGetPostsHandle(Client &client)
{
    bool running = true;
    while (running)
    {
        Menu posts_options_menu("What kind of posts would you like to see?", posts_options);
        int user_choice = posts_options_menu.print();
        switch (user_choice)
        {
        case 0:
        {
            guestGetPostsHandle(client);
            break;
        }
        case 1:
        {
            authenticatedUserGetGroupPostsHandle(client, "FRIENDS");
            break;
        }
        case 2:
        {
            authenticatedUserGetGroupPostsHandle(client, "FAMILY");
            break;
        }
        case 3:
        {
            running = false;
            break;
        }
        }
    }
}

void ClientApp::authenticatedUserCreatePostHandle(Client &client)
{
    string title, type, content;
    int user_choice;
    if ((title = readUsername("Press \"ESC\" to exit.\nEnter the title of your post: ")) == "ESC")
        return;
    Menu Post_type("What type of post do you want to create.", post_options);
    user_choice = Post_type.print();

    switch (user_choice)
    {
    case 0:
    {
        type = "PUBLIC";
        break;
    }
    case 1:
    {
        type = "FRIENDS";
        break;
    }
    case 2:
    {
        type = "FAMILY";
        break;
    }
    case 3:
    {
        return;
    }
    }
    bool confirming = true;
    while (confirming)
    {
        if ((content = readUsername("Press \"ESC\" to exit.\nPress \"Enter\" to continue.\nWrite the content of your post: ")) == "ESC")
            return;
        Menu confirm_menu("Are you sure you want to post this?", confirm_options);
        int confirm = confirm_menu.print();
        switch (confirm)
        {
        case 0:
        {
            confirming = false;
            break;
        }
        case 1:
        {
            break;
        }
        }
    }
    Command command("POSTCREATE", {title, content, type});
    client.sendCommand(*command);
    Response response = client.receiveResponse();
    if (response.getStatus() == "SUCCESS")
    {
        cout << "Post created successfully! Hit \"Enter\" to continue.";
        cin.ignore();
    }
    else
    {
        cout << "Post was not created!  Hit \"Enter\" to continue.";
        cin.ignore();
    }
}

void ClientApp::authenticatedUserAddFriendHandle(Client &client)
{
    bool running = true;
    while (running)
    {
        Menu friends_menu("What would you like to do?", friends_options);
        int user_choice = friends_menu.print();
        string friend_username;
        switch (user_choice)
        {
        case 0:
        {
            if ((friend_username = readUsername("Press \"ESC\" to exit.\nEnter your friend's username: ")) == "ESC")
                continue;
            break;
        }
        case 1:
        {
            friend_username = "ALL";
            break;
        }
        case 2:
        {
            running = false;
            continue;
        }
        }
        Command command("LOOKUP", {friend_username});
        client.sendCommand(*command);

        bool more = true;
        while (more)
        {
            Response response(client.receiveResponse());
            if (response.getStatus() == "SUCCESS")
            {
                int friends_number = stoi(response.getArgs()[0]);
                vector<string> friends;
                for (int i = 0; i < friends_number; i++)
                {
                    Response friends_response(client.receiveResponse());
                    friends.push_back(friends_response.getArgs()[0]);
                }
                friends.push_back("More users");
                friends.push_back("Exit");
                int exit_id = friends.size() - 1;
                int more_id = exit_id - 1;
                while (true)
                {
                    Menu users_menu("You can pick one of these users to add as a friend:", friends);
                    int user_choice = users_menu.print();
                    if (user_choice == exit_id)
                    {
                        Command command("DONE", {});
                        client.sendCommand(*command);
                        more = false;
                        break;
                    }
                    else if (user_choice == more_id)
                    {
                        Command command("MORE", {});
                        client.sendCommand(*command);
                        break;
                    }
                    else
                    {
                        string friend_type;
                        Menu friend_type_menu("What group should " + friends[user_choice] + " be added to?", friend_type_options);
                        int type_choice = friend_type_menu.print();
                        switch (type_choice)
                        {
                        case 0:
                        {
                            friend_type = "FRIEND";
                            break;
                        }
                        case 1:
                        {
                            friend_type = "FAMILY";
                            break;
                        }
                        case 2:
                        {
                            continue;
                        }
                        }
                        Menu confirm_menu("Are you sure you want to add " + friends[user_choice] + " to your friends list?", confirm_options);
                        int choice = confirm_menu.print();
                        if (choice == 0)
                        {
                            Command command("ADDFRIEND", {friends[user_choice], friend_type});
                            client.sendCommand(*command);
                            Response addresponse(client.receiveResponse());
                            system("clear");
                            if (addresponse.getStatus() == "SUCCESS")
                            {
                                cout << "Friend added successfully!\nHit \"Enter\" to continue.";
                                cin.ignore();
                                more = false;
                                break;
                            }
                            else
                            {
                                cout << "Friend was not added!\nHit \"Enter\" to continue.";
                                cin.ignore();
                                more = false;
                                break;
                            }
                        }
                    }
                }
            }
            else if (response.getStatus() == "NOFRIENDS")
            {
                system("clear");
                cout << "No users available! Hit \"Enter\" to continue.";
                cin.ignore();
                more = false;
            }
        }
    }
}

void ClientApp::authenticatedUserGetFriendsHandle(Client &client)
{
    Command command("LOOKUP", {"FRIENDS"});
    client.sendCommand(*command);
    bool more = true;
    while (more)
    {
        Response response(client.receiveResponse());
        if (response.getStatus() == "SUCCESS")
        {
            int friends_number = stoi(response.getArgs()[0]);
            vector<string> friends;
            for (int i = 0; i < friends_number; i++)
            {
                Response friends_response(client.receiveResponse());
                friends.push_back(friends_response.getArgs()[0]);
            }
            friends.push_back("More users");
            friends.push_back("Exit");
            int exit_id = friends.size() - 1;
            int more_id = exit_id - 1;
            while (true)
            {
                Menu users_menu("These are your friends:", friends);
                int user_choice = users_menu.print();
                if (user_choice == exit_id)
                {
                    Command command("DONE", {});
                    client.sendCommand(*command);
                    more = false;
                    break;
                }
                else if (user_choice == more_id)
                {
                    Command command("MORE", {});
                    client.sendCommand(*command);
                    break;
                }
                else
                {
                    authenticatedUserChatWithHandle(client, friends[user_choice]);
                    return;
                }
            }
        }
        else if (response.getStatus() == "NOFRIENDS")
        {
            system("clear");
            cout << "No users available! Hit \"Enter\" to continue.";
            cin.ignore();
            more = false;
        }
    }
}

void ClientApp::authenticatedUserChatWithHandle(Client &client, string friend_name)
{
    stringstream ss(friend_name);
    string frn;
    getline(ss, frn, ' ');
    Command command("CHATWITH", {frn});
    client.sendCommand(*command);
    while (true)
    {
        Response response(client.receiveResponse());
        if (response.getStatus() == "SUCCESS")
        {
            system("clear");
            cout << "Now chatting with your friend.\n";
            int message_number = stoi(response.getArgs()[0]);
            cout << '\n';
            vector<string> messages;
            for (int i = 0; i < message_number; i++)
            {
                Response message_response(client.receiveResponse());
                messages.push_back(message_response.getArgs()[0]);
            }
            for (int i = 0; i < messages.size(); i++)
            {
                cout << messages[i] << '\n';
            }
            cout << '\n';
            string message;
            if ((message = readMessage("Press \"ESC\" to exit.\nEnter your message: ")) == "ESC")
            {
                Command end("DONE", {});
                client.sendCommand(*end);
                return;
            }
            if (message.length() != 0)
            {
                Command message_command("MESSAGE", {message});
                client.sendCommand(*message_command);
            }
            else
            {
                Command message_command("REFRESH", {});
                client.sendCommand(*message_command);
            }
        }
    }
}

void ClientApp::Start()
{
    Client client(server_address, port);
    while (running)
    {
        int user_choice;
        if (current_user->getType() == "GUEST")
        {
            Menu main_menu = Menu("Welcome " + current_user->getUsername() + "!", main_menu_guest_options);
            user_choice = main_menu.print();
            switch (user_choice)
            {
            case 3:
            {
                running = Close(client);
                break;
            }
            case 0:
            {
                User *responseUser = guestLoginHandle(client);
                if (responseUser != nullptr)
                {
                    delete current_user;
                    current_user = responseUser;
                }

                break;
            }
            case 1:
            {
                User *responseUser = guestRegisterHandle(client);
                if (responseUser != nullptr)
                {
                    delete current_user;
                    current_user = responseUser;
                }
                break;
            }
            case 2:
            {
                guestGetPostsHandle(client);
                break;
            }
            }
        }
        else if (current_user->getType() == "REGULAR")
        {
            Menu main_menu = Menu("Welcome " + current_user->getUsername() + "!", main_menu_authenticated_user_options);
            user_choice = main_menu.print();
            switch (user_choice)
            {
            case 5:
            {
                running = Close(client);
                break;
            }
            case 4:
            {
                User *response_user = authenticatedUserLogoutHandle(client);
                if (response_user != nullptr)
                {
                    delete current_user;
                    current_user = response_user;
                }
                break;
            }
            case 1:
            {
                authenticatedUserGetPostsHandle(client);
                break;
            }
            case 2:
            {
                authenticatedUserGetFriendsHandle(client);
                break;
            }
            case 3:
            {
                authenticatedUserAddFriendHandle(client);
                break;
            }
            case 0:
            {
                authenticatedUserCreatePostHandle(client);
                break;
            }
            }
        }
        else if (current_user->getType() == "ADMIN")
        {
            Menu main_menu("Welcome " + current_user->getUsername() + "!", main_menu_administrator_options);
            int user_choice = main_menu.print();
            switch (user_choice)
            {
            case 0:
            {
                administratorDeletePostHandle(client);
                cout << "Exited" << endl;
                break;
            }
            case 1:
            {
                User *response_user = authenticatedUserLogoutHandle(client);
                if (response_user != nullptr)
                {
                    delete current_user;
                    current_user = response_user;
                }
                break;
            }
            case 2:
            {
                running = Close(client);
                break;
            }
            }
        }
    }
}