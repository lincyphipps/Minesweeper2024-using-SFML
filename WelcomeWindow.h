#ifndef MINESWEEPER_WELCOMEWINDOW_H
#define MINESWEEPER_WELCOMEWINDOW_H

#include <SFML/Graphics.hpp>
#include <fstream>
#include <iostream>
#include <sstream>
#include "GameWindow.h"
using namespace std;

class WelcomeWindow {
    float width;
    float height;
    int mine_count;
public:
    WelcomeWindow(int width, int height, int mine_count) {
        this->width = width;
        this->height = height;
        this->mine_count = mine_count;
    }
    void setText(sf::Text &text, float x, float y) {
        sf::FloatRect textRect = text.getLocalBounds();
        text.setOrigin(textRect.left + textRect.width / 2.0f, textRect.top + textRect.height / 2.0f);
        text.setPosition(sf::Vector2f(x, y));
    }
    void run(sf::RenderWindow &welcome_window) {
        sf::Font font;
        if (!font.loadFromFile("files/font.ttf")) {cerr << "Could not load font file." << endl;}
        sf::Text welc_text;
        welc_text.setFont(font);
        welc_text.setString("WELCOME TO MINESWEEPER!");
        welc_text.setCharacterSize(24);
        welc_text.setFillColor(sf::Color::White);
        welc_text.setStyle(sf::Text::Bold | sf::Text::Underlined);
        setText(welc_text, width / 2, (height / 2) - 150);

        sf::Text name_enter_text;
        name_enter_text.setFont(font);
        name_enter_text.setString("Enter your name:");
        name_enter_text.setCharacterSize(20);
        name_enter_text.setFillColor(sf::Color::White);
        name_enter_text.setStyle(sf::Text::Bold);
        setText(name_enter_text, width / 2, (height / 2) - 75);

        sf::Text name_text;
        name_text.setFont(font);
        name_text.setString("|");
        name_text.setCharacterSize(18);
        name_text.setFillColor(sf::Color::Yellow);
        name_text.setStyle(sf::Text::Bold);
        setText(name_text, width / 2, (height / 2) - 45);
        int count = 0;
        string name = "";

        while (welcome_window.isOpen()) {
            sf::Event event;
            while (welcome_window.pollEvent(event)) {
                if (event.type == sf::Event::Closed) {welcome_window.close();}
                if (event.type == sf::Event::TextEntered) {
                    if (event.text.unicode == 8) {
                        name = name.substr(0,name.length()-1);
                        name_text.setString(name + '|');
                        setText(name_text, width / 2, (height / 2) - 45);
                        count--;
                    }
                    else if (count < 10) {
                        char chr = static_cast<char>(event.text.unicode);
                        if (isalpha(chr)) {
                            chr = count == 0 ? (char) toupper(chr) : (char) tolower(chr);
                            name += chr;
                            count++;
                        }
                        name_text.setString(name + '|');
                        setText(name_text, width / 2, (height / 2) - 45);
                    }
                }
                if (event.type == sf::Event::KeyPressed) {
                    if ((event.key.code == sf::Keyboard::Return)&&(count>=1)) {
                        welcome_window.close();
                    }
                }
            }
            welcome_window.clear(sf::Color::Blue);
            welcome_window.draw(welc_text);
            welcome_window.draw(name_enter_text);
            welcome_window.draw(name_text);
            welcome_window.display();
        }
        if (!name.empty()) {
            sf::RenderWindow game_window(sf::VideoMode((int) width, (int) height), "Minesweeper", sf::Style::Titlebar | sf::Style::Close);
            GameWindow game(name, width, height, mine_count);
            game.run(game_window);
        }
    }
};

#endif //MINESWEEPER_WELCOMEWINDOW_H