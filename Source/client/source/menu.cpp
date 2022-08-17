#include "../headers/menu.h"
#include <iostream>
#include <termios.h>
#include <unistd.h>

// Credit to user _mf on Stack Overflow for the getch function on linux
char getch(void)
{
    char buf = 0;
    struct termios old = {0};
    fflush(stdout);
    if (tcgetattr(0, &old) < 0)
        perror("tcsetattr()");
    old.c_lflag &= ~ICANON;
    old.c_lflag &= ~ECHO;
    old.c_cc[VMIN] = 1;
    old.c_cc[VTIME] = 0;
    if (tcsetattr(0, TCSANOW, &old) < 0)
        perror("tcsetattr ICANON");
    if (read(0, &buf, 1) < 0)
        perror("read()");
    old.c_lflag |= ICANON;
    old.c_lflag |= ECHO;
    if (tcsetattr(0, TCSADRAIN, &old) < 0)
        perror("tcsetattr ~ICANON");
    return buf;
}

Menu::Menu(string _header, vector<string> &constructor_vect)
{
    content = constructor_vect;
    header = _header;
    selector.setMax(content.size() - 1);
}

int Menu::print()
{
    system("clear");
    int keypress = 0;
    while (keypress != 10)
    {
        cout << header << endl;
        for (int i = 0; i < content.size(); i++)
        {
            if (i == selector.getValue())
                cout << "\033[1;42m" << content[i] << "\033[0m\n";
            else
                cout << content[i] << '\n';
        }
        keypress = getch();
        if (keypress == 66)
            ++selector;
        if (keypress == 65)
            --selector;
        system("clear");
    }
    return selector.getValue();
}