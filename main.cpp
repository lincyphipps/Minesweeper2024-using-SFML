#include <iostream>
#include <SFML/Graphics.hpp>
#include <fstream>
#include "WelcomeWindow.h"
using namespace std;

int main() {
    // read the config file to make size of windows
    ifstream configfile("files/config.cfg");
    if (!configfile.is_open()) {
        cerr << "File is not open!";
    }
    string read;
    int row_count, col_count, mine_count;
    while(!configfile.eof()) {
        string token;
        getline(configfile, token);
        col_count = stoi(token);
        getline(configfile, token);
        row_count = stoi(token);
        getline(configfile, token);
        mine_count = stoi(token);
    }
    float width = (float) col_count*32;
    float height = ((float) row_count*32) + 100;

    sf::RenderWindow window(sf::VideoMode((int)width, (int) height), "SFML window", sf::Style::Close);

    // do the welcome screen
    WelcomeWindow welcomeWindow(width, height, mine_count);
    welcomeWindow.run(window);



    return 0;
}