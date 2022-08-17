#include "../headers/databaseManager.h"
#include <sqlite3.h>
#include <cstring>
#include <iostream>
#include <algorithm>
DatabaseManager::DatabaseManager()
{
    sqlite3 *cursor;
    string sqlStatement = "CREATE TABLE IF NOT EXISTS USERS(USERNAME TEXT, PASSWORD TEXT, TYPE TEXT);"
                          "CREATE TABLE IF NOT EXISTS POSTS(TITLE TEXT, AUTHOR TEXT, CONTENT BLOB, TYPE TEXT, DATE INTEGER);"
                          "CREATE TABLE IF NOT EXISTS FRIENDS(USER_ID INTEGER, FRIEND_ID INTEGER, TYPE TEXT);"
                          "INSERT INTO USERS VALUES('ADMINISTRATOR', '03f70d78376b41fe748f5414954cf4cad830dd6ad583c2994b5ebb1bc12d2e028827fce6396bc4f6ab0d584f725748b30629468d87fe4bdd2a37bdf726300066', 'ADMIN')";
    char *zErrMsg = 0;

    int rc = sqlite3_open("ServerDatabase.db", &cursor);

    if (rc)
    {

        cout << "Can't open database " << sqlite3_errmsg(cursor) << endl;
        exit(-1);
    }
    else
    {
        cout << "Database opened successfully" << endl;
    }

    rc = sqlite3_exec(cursor, sqlStatement.c_str(), nullptr, 0, &zErrMsg);

    if (rc != SQLITE_OK)
    {
        cout << "SQL error: " << zErrMsg << endl;
        sqlite3_free(zErrMsg);
    }
    else
    {
        cout << "Table created successfully" << endl;
    }
    sqlite3_close(cursor);
}

bool DatabaseManager::checkIfUsernameExists(string username)
{
    bool output = true;
    sqlite3 *cursor;
    string sqlStatement = "SELECT rowid FROM USERS WHERE username='" + username + "'";
    char *zErrMsg = 0;

    int rc = sqlite3_open("ServerDatabase.db", &cursor);

    if (rc)
    {

        cout << "Can't open database " << sqlite3_errmsg(cursor) << endl;
        exit(-1);
    }
    else
    {
        cout << "Database opened successfully" << endl;
    }

    auto callback = [](void *data, int argc, char **argv, char **azColName)
    {
        vector<string> *aux = (vector<string> *)data;
        for (int i = 0; i < argc; i++)
            aux->push_back(argv[i]);
        return 0;
    };
    vector<string> selectOutput;
    rc = sqlite3_exec(cursor, sqlStatement.c_str(), callback, &selectOutput, &zErrMsg);

    if (rc != SQLITE_OK)
    {
        cout << "SQL error: " << zErrMsg << endl;
        sqlite3_free(zErrMsg);
    }
    else
    {
        cout << "Query successfull" << endl;
        if (selectOutput.size() == 0)
        {
            output = false;
        }
    }
    sqlite3_close(cursor);
    return output;
}

User *DatabaseManager::getRegisteredUser(string username, string password)
{
    User *output = nullptr;
    sqlite3 *cursor;
    string sqlStatement = "SELECT rowid, username, type FROM USERS WHERE username='" + username + "' AND password='" + password + "'";
    char *zErrMsg = 0;

    int rc = sqlite3_open("ServerDatabase.db", &cursor);

    if (rc)
    {

        cout << "Can't open database " << sqlite3_errmsg(cursor) << endl;
        exit(-1);
    }
    else
    {
        cout << "Database opened successfully" << endl;
    }

    auto callback = [](void *data, int argc, char **argv, char **azColName)
    {
        vector<User> *aux = (vector<User> *)data;
        int id = atoi(argv[0]);
        string username = argv[1];
        string type = argv[2];
        aux->push_back(User(id, username, type));
        return 0;
    };
    vector<User> selectOutput;
    rc = sqlite3_exec(cursor, sqlStatement.c_str(), callback, &selectOutput, &zErrMsg);

    if (rc != SQLITE_OK)
    {
        cout << "SQL error: " << zErrMsg << endl;
        sqlite3_free(zErrMsg);
    }
    else
    {
        cout << "Query successfull" << endl;
        if (selectOutput.size() == 1)
        {
            output = new User(selectOutput[0]);
        }
    }
    sqlite3_close(cursor);
    return output;
}

int DatabaseManager::addNewUserToDatabase(string username, string password, string type)
{
    sqlite3 *cursor;
    string sqlStatement = "INSERT INTO USERS (username, password, type) VALUES ('" + username + "','" + password + "','" + type + "')";
    char *zErrMsg = 0;

    int rc = sqlite3_open("ServerDatabase.db", &cursor);

    if (rc)
    {
        cout << "Can't open database " << sqlite3_errmsg(cursor) << endl;
        exit(-1);
    }
    else
    {
        cout << "Database opened successfully" << endl;
    }

    rc = sqlite3_exec(cursor, sqlStatement.c_str(), nullptr, nullptr, &zErrMsg);

    if (rc != SQLITE_OK)
    {
        cout << "SQL error: " << zErrMsg << endl;
        sqlite3_free(zErrMsg);
    }
    else
    {
        cout << "Insert successfull" << endl;
    }
    int rowid = sqlite3_last_insert_rowid(cursor);
    sqlite3_close(cursor);
    return rowid;
}

vector<Post> DatabaseManager::getPosts(string type)
{
    vector<Post> selectOutput;
    sqlite3 *cursor;
    string sqlStatement;
    if (type == "ALL")
    {
        sqlStatement = "SELECT rowid, * FROM POSTS ORDER BY date ASC;";
    }
    else
    {
        sqlStatement = "SELECT rowid, * FROM POSTS WHERE type='" + type + "' ORDER BY date ASC";
    }
    char *zErrMsg = 0;

    int rc = sqlite3_open("ServerDatabase.db", &cursor);

    if (rc)
    {

        cout << "Can't open database " << sqlite3_errmsg(cursor) << endl;
        exit(-1);
    }
    else
    {
        cout << "Database opened successfully" << endl;
    }

    auto callback = [](void *data, int argc, char **argv, char **azColName)
    {
        vector<Post> *aux = (vector<Post> *)data;
        int id = atoi(argv[0]);
        string title = argv[1];
        string author = argv[2];
        string content = argv[3];
        string type = argv[4];
        string date = argv[5];
        aux->push_back(Post(id, title, author, content, type, date));
        return 0;
    };
    rc = sqlite3_exec(cursor, sqlStatement.c_str(), callback, &selectOutput, &zErrMsg);

    if (rc != SQLITE_OK)
    {
        cout << "SQL error: " << zErrMsg << endl;
        sqlite3_free(zErrMsg);
    }
    else
    {
        cout << "Query successfull" << endl;
    }
    sqlite3_close(cursor);
    return selectOutput;
}

vector<Post> DatabaseManager::getSpecificPosts(string type, int id)
{
    vector<Post> selectOutput;
    sqlite3 *cursor;
    string sqlStatement = "select p.rowid, p.title, p.author, p.content, p.type, p.date from posts p join users u on u.username = p.author join friends f on u.rowid = f.friend_id where f.user_id = " + to_string(id) + " and p.type = '" + type + "';";
    char *zErrMsg = 0;

    int rc = sqlite3_open("ServerDatabase.db", &cursor);

    if (rc)
    {
        cout << "Can't open database " << sqlite3_errmsg(cursor) << endl;
        exit(-1);
    }
    else
    {
        cout << "Database opened successfully" << endl;
    }

    auto callback = [](void *data, int argc, char **argv, char **azColName)
    {
        vector<Post> *aux = (vector<Post> *)data;
        int id = atoi(argv[0]);
        string title = argv[1];
        string author = argv[2];
        string content = argv[3];
        string type = argv[4];
        string date = argv[5];
        aux->push_back(Post(id, title, author, content, type, date));
        return 0;
    };
    rc = sqlite3_exec(cursor, sqlStatement.c_str(), callback, &selectOutput, &zErrMsg);

    if (rc != SQLITE_OK)
    {
        cout << "SQL error: " << zErrMsg << endl;
        sqlite3_free(zErrMsg);
    }
    else
    {
        cout << "Query successfull" << endl;
    }
    sqlite3_close(cursor);
    return selectOutput;
}

vector<string> DatabaseManager::lookupUsernameNotFriend(int id, string username)
{
    vector<string> selectOutput;
    sqlite3 *cursor;
    std::for_each(username.begin(), username.end(), [](char &c)
                  { c = ::tolower(c); });
    string sqlStatement = "select name from"
                          "(select u.rowid, u.username as name from users u where u.type <> 'ADMIN' and u.rowid <> " +
                          to_string(id) +
                          " except "
                          " select f.friend_id, u.username as name from friends f join users u on u.rowid = f.friend_id where f.user_id = " +
                          to_string(id) +
                          ") where LOWER(name) like '%" + username + "%'";
    char *zErrMsg = 0;

    int rc = sqlite3_open("ServerDatabase.db", &cursor);

    if (rc)
    {

        cout << "Can't open database " << sqlite3_errmsg(cursor) << endl;
        exit(-1);
    }
    else
    {
        cout << "Database opened successfully" << endl;
    }

    auto callback = [](void *data, int argc, char **argv, char **azColName)
    {
        vector<string> *aux = (vector<string> *)data;
        string username = argv[0];
        aux->push_back(username);
        return 0;
    };
    rc = sqlite3_exec(cursor, sqlStatement.c_str(), callback, &selectOutput, &zErrMsg);

    if (rc != SQLITE_OK)
    {
        cout << "SQL error: " << zErrMsg << endl;
        sqlite3_free(zErrMsg);
    }
    else
    {
        cout << "Query successfull" << endl;
    }
    sqlite3_close(cursor);
    return selectOutput;
}

vector<string> DatabaseManager::lookupFriend(int id)
{
    vector<string> selectOutput;
    sqlite3 *cursor;
    string sqlStatement = "select u.username, f.type from friends f join users u on u.rowid = f.friend_id where f.user_id = " + to_string(id) + ";";
    char *zErrMsg = 0;

    int rc = sqlite3_open("ServerDatabase.db", &cursor);

    if (rc)
    {

        cout << "Can't open database " << sqlite3_errmsg(cursor) << endl;
        exit(-1);
    }
    else
    {
        cout << "Database opened successfully" << endl;
    }

    auto callback = [](void *data, int argc, char **argv, char **azColName)
    {
        vector<string> *aux = (vector<string> *)data;
        string username = argv[0];
        username += " - ";
        username += argv[1];
        aux->push_back(username);
        return 0;
    };
    rc = sqlite3_exec(cursor, sqlStatement.c_str(), callback, &selectOutput, &zErrMsg);

    if (rc != SQLITE_OK)
    {
        cout << "SQL error: " << zErrMsg << endl;
        sqlite3_free(zErrMsg);
    }
    else
    {
        cout << "Query successfull" << endl;
    }
    sqlite3_close(cursor);
    return selectOutput;
}

int DatabaseManager::getUserByUsername(string username)
{
    int selectOutput;
    sqlite3 *cursor;
    string sqlStatement = "SELECT rowid FROM users WHERE username = '" + username + "'";
    char *zErrMsg = 0;

    int rc = sqlite3_open("ServerDatabase.db", &cursor);

    if (rc)
    {

        cout << "Can't open database " << sqlite3_errmsg(cursor) << endl;
        exit(-1);
    }
    else
    {
        cout << "Database opened successfully" << endl;
    }

    auto callback = [](void *data, int argc, char **argv, char **azColName)
    {
        int *aux = (int *)data;
        *aux = atoi(argv[0]);
        return 0;
    };
    rc = sqlite3_exec(cursor, sqlStatement.c_str(), callback, &selectOutput, &zErrMsg);

    if (rc != SQLITE_OK)
    {
        cout << "SQL error: " << zErrMsg << endl;
        sqlite3_free(zErrMsg);
    }
    else
    {
        cout << "Query successfull" << endl;
    }
    sqlite3_close(cursor);
    return selectOutput;
}

void DatabaseManager::addFriend(int user_id, int friend_id, string type)
{
    sqlite3 *cursor;
    string sqlStatement = "INSERT INTO FRIENDS (user_id, friend_id, type) VALUES (" + to_string(user_id) + "," + to_string(friend_id) + ",'" + type + "');"
                                                                                                                                                      "INSERT INTO FRIENDS (user_id, friend_id, type) VALUES (" +
                          to_string(friend_id) + "," + to_string(user_id) + ",'" + type + "')";
    char *zErrMsg = 0;

    int rc = sqlite3_open("ServerDatabase.db", &cursor);

    if (rc)
    {
        cout << "Can't open database " << sqlite3_errmsg(cursor) << endl;
        exit(-1);
    }
    else
    {
        cout << "Database opened successfully" << endl;
    }

    rc = sqlite3_exec(cursor, sqlStatement.c_str(), nullptr, nullptr, &zErrMsg);

    if (rc != SQLITE_OK)
    {
        cout << "SQL error: " << zErrMsg << endl;
        sqlite3_free(zErrMsg);
    }
    else
    {
        cout << "Insert successfull" << endl;
    }
    sqlite3_close(cursor);
}

void DatabaseManager::addPost(string title, string author, string content, string type)
{
    sqlite3 *cursor;
    string sqlStatement = "INSERT INTO POSTS VALUES ('" + title + "','" + author + "','" + content + "','" + type + "'," + to_string(time(nullptr)) + ");";
    char *zErrMsg = 0;

    int rc = sqlite3_open("ServerDatabase.db", &cursor);

    if (rc)
    {
        cout << "Can't open database " << sqlite3_errmsg(cursor) << endl;
        exit(-1);
    }
    else
    {
        cout << "Database opened successfully" << endl;
    }

    rc = sqlite3_exec(cursor, sqlStatement.c_str(), nullptr, nullptr, &zErrMsg);

    if (rc != SQLITE_OK)
    {
        cout << "SQL error: " << zErrMsg << endl;
        sqlite3_free(zErrMsg);
    }
    else
    {
        cout << "Insert successfull" << endl;
    }
    sqlite3_close(cursor);
}

void DatabaseManager::deletePost(int id)
{
    sqlite3 *cursor;
    string sqlStatement = "DELETE FROM posts WHERE rowid = " + to_string(id) + ";";
    char *zErrMsg = 0;

    int rc = sqlite3_open("ServerDatabase.db", &cursor);

    if (rc)
    {
        cout << "Can't open database " << sqlite3_errmsg(cursor) << endl;
        exit(-1);
    }
    else
    {
        cout << "Database opened successfully" << endl;
    }

    rc = sqlite3_exec(cursor, sqlStatement.c_str(), nullptr, nullptr, &zErrMsg);

    if (rc != SQLITE_OK)
    {
        cout << "SQL error: " << zErrMsg << endl;
        sqlite3_free(zErrMsg);
    }
    else
    {
        cout << "Insert successfull" << endl;
    }
    sqlite3_close(cursor);
}

vector<string> DatabaseManager::getMessages(int id, int idf)
{
    vector<string> selectOutput;
    sqlite3 *cursor;
    string tableName = "TB" + to_string(min(id, idf)) + "__" + to_string(max(id, idf));
    string sqlStatement1 = "CREATE TABLE IF NOT EXISTS " + tableName + "(SENDER TEXT, MESSAGE BLOB, DATE INTEGER);";
    string sqlStatement = "SELECT * FROM " + tableName + " ORDER BY DATE ASC;";
    char *zErrMsg = 0;

    int rc = sqlite3_open("ServerDatabase.db", &cursor);

    if (rc)
    {
        cout << "Can't open database " << sqlite3_errmsg(cursor) << endl;
        exit(-1);
    }
    else
    {
        cout << "Database opened successfully" << endl;
    }

    rc = sqlite3_exec(cursor, sqlStatement1.c_str(), nullptr, 0, &zErrMsg);

    auto callback = [](void *data, int argc, char **argv, char **azColName)
    {
        vector<string> *aux = (vector<string> *)data;
        string sender = argv[0];
        string message = argv[1];
        string date;
        time_t time = stoi(argv[2]);
        date = asctime(localtime(&time));
        date.pop_back();
        aux->push_back(sender + " on " + date + ": " + message);
        return 0;
    };
    rc = sqlite3_exec(cursor, sqlStatement.c_str(), callback, &selectOutput, &zErrMsg);
    cout << "Executed the select\n";
    if (rc != SQLITE_OK)
    {
        cout << "SQL error: " << zErrMsg << endl;
        sqlite3_free(zErrMsg);
    }
    else
    {
        cout << "Query successfull" << endl;
    }
    sqlite3_close(cursor);
    return selectOutput;
}

void DatabaseManager::addMessage(int id, int idf, string sender, string message)
{
    sqlite3 *cursor;
    string tableName = "TB" + to_string(min(id, idf)) + "__" + to_string(max(id, idf));
    string rebuild = "";
    for (int i = 0; i < message.length(); i++)
    {
        rebuild += message[i];
        if (message[i] == '\'')
            rebuild += '\'';
    }
    string sqlStatement = "INSERT INTO " + tableName + " VALUES ('" + sender + "', '" + rebuild + "', " + to_string(time(nullptr)) + ");";
    cout << sqlStatement << '\n';
    char *zErrMsg = 0;

    int rc = sqlite3_open("ServerDatabase.db", &cursor);

    if (rc)
    {
        cout << "Can't open database " << sqlite3_errmsg(cursor) << endl;
        exit(-1);
    }
    else
    {
        cout << "Database opened successfully" << endl;
    }

    rc = sqlite3_exec(cursor, sqlStatement.c_str(), nullptr, nullptr, &zErrMsg);

    if (rc != SQLITE_OK)
    {
        cout << "SQL error: " << zErrMsg << endl;
        sqlite3_free(zErrMsg);
    }
    else
    {
        cout << "Insert successfull" << endl;
    }
    sqlite3_close(cursor);
}
