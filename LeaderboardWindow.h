#ifndef MINESWEEPER_LEADERBOARDWINDOW_H
#define MINESWEEPER_LEADERBOARDWINDOW_H
#include <SFML/Graphics.hpp>
#include <fstream>
#include <iostream>
#include <sstream>
#include <iomanip>
#include <string>
#include "GameWindow.h"
using namespace std;

class LeaderboardWindow {
    float width;
    float height;
    string name;
    sf::Font font;
public:
    LeaderboardWindow(string name, int width, int height) {
        this->name = name;
        this->width = width;
        this->height = height;
        font.loadFromFile("files/font.ttf");
    }
    void setText(sf::Text &text, float x, float y) {
        sf::FloatRect textRect = text.getLocalBounds();
        text.setOrigin(textRect.left + textRect.width / 2.0f, textRect.top + textRect.height / 2.0f);
        text.setPosition(sf::Vector2f(x, y));
    }
    void run(sf::RenderWindow& leader, string content, Board &board, sf::RenderWindow& game) {
        sf::Text leader_text;
        leader_text.setFont(font);
        leader_text.setString("LEADERBOARD");
        leader_text.setCharacterSize(20);
        leader_text.setFillColor(sf::Color::White);
        leader_text.setStyle(sf::Text::Bold | sf::Text::Underlined);
        setText(leader_text, width / 2, (height / 2) - 120);

        sf::Text entry_text;
        entry_text.setFont(font);
        entry_text.setString(content);
        entry_text.setCharacterSize(18);
        entry_text.setFillColor(sf::Color::White);
        entry_text.setStyle(sf::Text::Bold);
        setText(entry_text, width / 2, (height / 2) + 20);
        leader.draw(entry_text);

        board.leaderOn = true;
        while(leader.isOpen()) {
            sf::Event event;
            board.leaderOn = true;
            while (leader.pollEvent(event)) {
                board.leaderOn = true;
                if (event.type == sf::Event::Closed) {
                    leader.close();
                    board.leaderOn = false;
                }
            }
            leader.clear(sf::Color::Blue);
            leader.draw(leader_text);
            leader.draw(entry_text);
            leader.display();
        }
    }
};


#endif //MINESWEEPER_LEADERBOARDWINDOW_H
