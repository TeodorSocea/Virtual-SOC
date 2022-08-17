#pragma once

#include <string>
#include <vector>

using namespace std;

vector<string> main_menu_guest_options = {"Log in", "Register", "View Posts", "Close Application"};

vector<string> main_menu_authenticated_user_options = {"Create Post", "View Posts", "View Friends", "Add Friend", "Log out", "Close Application"};

vector<string> main_menu_administrator_options = {"Delete Post", "Log Out", "Close Application"};

vector<string> posts_options = {"Public Posts", "Friends' Posts", "Family's Posts", "Exit"};

vector<string> friends_options = {"Search for a friend", "View users", "Exit"};

vector<string> confirm_options = {"Yes", "No"};

vector<string> post_options = {"Public", "Friends", "Family", "Exit"};

vector<string> friend_type_options = {"Friends", "Family", "Exit"};
