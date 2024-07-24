#ifndef MINESWEEPER_GAMEWINDOW_H
#define MINESWEEPER_GAMEWINDOW_H

#include <SFML/Graphics.hpp>
#include <fstream>
#include <iostream>
#include <sstream>
#include <string>
#include <vector>
#include <random>
#include <iterator>
#include <map>
#include "Tile.h"
#include "Board.h"
#include "LeaderboardWindow.h"
using namespace std;

class GameWindow {
    string name;
    float width;
    float height;
    int mine_count;
    sf::Clock clock;
    sf::Time elapsed_time;
    int secs;
    int mins;
    bool debug = false;
    bool playing = true;
    bool winStatus = true;
    bool won = false;
    bool lost = false;
    bool canPause = true;
public:
    string content_str;
    map<int, string> leaderboard;
    GameWindow(string name, int width, int height, int mine_count) {
        this->name = name;
        this->width = width;
        this->height = height;
        this->mine_count = mine_count;
    }
    void run(sf::RenderWindow& game) {
        Board board(width/32, (height - 100) / 32, mine_count);
        board.blankBoard();
        board.makeMines();

        sf::Texture happy_face_texture;
        happy_face_texture.loadFromFile("files/images/face_happy.png");
        sf::Sprite happy_face_sprite(happy_face_texture);
        happy_face_sprite.setPosition(((board.col_count/2.0)*32)-32, 32*(board.row_count+0.5));

        sf::Texture debug_texture;
        debug_texture.loadFromFile("files/images/debug.png");
        sf::Sprite debug_button(debug_texture);
        debug_button.setPosition((board.col_count * 32) - 304, 32 * (board.row_count + 0.5));

        sf::Texture leader_texture;
        leader_texture.loadFromFile("files/images/leaderboard.png");
        sf::Sprite leader_sprite(leader_texture);
        leader_sprite.setPosition((board.col_count * 32) - 176, 32 * (board.row_count + 0.5));

        sf::Texture pause_texture;
        pause_texture.loadFromFile("files/images/pause.png");
        sf::Sprite pause_sprite(pause_texture);
        pause_sprite.setPosition((board.col_count * 32) - 240, 32 * (board.row_count + 0.5));

        sf::Texture play_texture;
        play_texture.loadFromFile("files/images/play.png");
        sf::Sprite play_sprite(play_texture);
        play_sprite.setPosition((board.col_count * 32) - 240, 32 * (board.row_count + 0.5));

        sf::Texture win_texture;
        win_texture.loadFromFile("files/images/face_win.png");
        sf::Sprite win_sprite(win_texture);
        win_sprite.setPosition(((board.col_count/2.0)*32)-32, 32*(board.row_count+0.5));

        sf::Texture lose_texture;
        lose_texture.loadFromFile("files/images/face_lose.png");
        sf::Sprite lose_sprite(lose_texture);
        lose_sprite.setPosition(((board.col_count/2.0)*32)-32, 32*(board.row_count+0.5));

        board.loadLeaderboard("files/leaderboard.txt", leaderboard);
        content_str = board.contentstr(leaderboard, mins * 60 + secs, name);

        while(game.isOpen()) {
            sf::Event event;
            while (game.pollEvent(event)) {
                if (event.type == sf::Event::Closed) {
                    game.close();
                }
                if (sf::Mouse::isButtonPressed(sf::Mouse::Left)) {
                    sf::Vector2i mouse_pos = sf::Mouse::getPosition(game);
                    if (debug_button.getGlobalBounds().contains(mouse_pos.x, mouse_pos.y) && playing) {
                        debug = !debug;
                    } else if (pause_sprite.getGlobalBounds().contains(mouse_pos.x, mouse_pos.y) && ((winStatus && playing) || canPause)) {
                        if (playing) {
                            elapsed_time += sf::seconds(clock.getElapsedTime().asSeconds());
                            debug = false;
                        } else {
                            clock.restart();
                        }
                        playing = !playing;
                    } else if (happy_face_sprite.getGlobalBounds().contains(mouse_pos.x, mouse_pos.y)) {
                        board.blankBoard();
                        board.mine_count = mine_count;
                        board.makeMines();
                        debug = false;
                        playing = true;
                        winStatus = true;
                        canPause = true;
                        board.leaderOn = false;
                        won = false;
                        lost = false;
                        clock.restart();
                        elapsed_time = sf::Time::Zero;
                    } else if (leader_sprite.getGlobalBounds().contains(mouse_pos.x, mouse_pos.y)) {
                        if (playing) {
                            elapsed_time += sf::seconds(clock.getElapsedTime().asSeconds());
                        } else {
                            clock.restart();
                        }
                        board.leaderOn = true;
                        playing = false;
                    } else {
                        if (playing && (mouse_pos.y < height)) {
                            if (!board.reveal(mouse_pos.x, mouse_pos.y, game) && playing) {
                                debug = true;
                                playing = false;
                                winStatus = false;
                                canPause = false;
                                lost = true;
                            }
                        }
                    }
                }
                if (sf::Mouse::isButtonPressed(sf::Mouse::Right)) {
                    sf::Vector2i mouse_pos = sf::Mouse::getPosition(game);
                    if ((mouse_pos.y < height) && (playing)) {
                        board.toggleFlag(mouse_pos.x, mouse_pos.y);
                    }
                }
            }
            if (playing) { elapsed_time += sf::seconds(clock.restart().asSeconds()); }
            secs = elapsed_time.asSeconds();
            mins = secs / 60;
            secs %= 60;

            game.clear(sf::Color::White);
            game.draw(debug_button);
            if (winStatus) { game.draw(happy_face_sprite); }
            game.draw(leader_sprite);
            board.drawDigs(game);
            board.drawClock(game, mins, secs);

            if (playing) { game.draw(pause_sprite); } else { game.draw(play_sprite); }

            for (int i = 0; i < board.row_count; i++) {
                for (int j = 0; j < board.col_count; j++) {
                    if (board.board_vect[i][j].is_revealed && !board.board_vect[i][j].is_flag) {
                        game.draw(board.board_vect[i][j].revealed_tile);
                        game.draw(board.board_vect[i][j].num_sprite);
                    } else {
                        board.board_vect[i][j].hidden_tile.setTexture(board.board_vect[i][j].hidden_tile_texture);
                        game.draw(board.board_vect[i][j].hidden_tile);
                        if (board.board_vect[i][j].is_flag) {
                            game.draw(board.board_vect[i][j].flag_sprite);
                        }
                    }
                }
            }
            if (!playing && canPause) {
                for (int i = 0; i < board.row_count; i++) {
                    for (int j = 0; j < board.col_count; j++) {
                        board.board_vect[i][j].cover_texture.loadFromFile("files/images/tile_revealed.png");
                        board.board_vect[i][j].cover_sprite.setTexture(board.board_vect[i][j].cover_texture);
                        board.board_vect[i][j].cover_sprite.setPosition(j * 32, i * 32);
                        game.draw(board.board_vect[i][j].cover_sprite);
                    }
                }
            }
            if (board.leaderOn) {
                for (int i = 0; i < board.row_count; i++) {
                    for (int j = 0; j < board.col_count; j++) {
                        game.draw(board.board_vect[i][j].revealed_tile);
                    }
                }
                game.display();
                sf::RenderWindow leader(sf::VideoMode((int)(width-100)/2 + 50, (int) height / 2), "SFML window", sf::Style::Close);
                LeaderboardWindow leader_window(name, (width - 100) / 2 + 50, height / 2);
                board.loadLeaderboard("files/leaderboard.txt", leaderboard);

                leader_window.run(leader, content_str, board, game);
                clock.restart();
                canPause = false;
                if (winStatus) { playing = false; }
                if (!board.leaderOn) {
                    if (winStatus) { playing = true; } else { playing = false; }
                }
            }
            if (board.checkWin()) {
                canPause = false;
                playing = false;
                winStatus = false;
                won = true;
                // update leaderboard
                board.loadLeaderboard("files/leaderboard.txt", leaderboard);

                leaderboard[mins * 60 + secs] = name;
                ofstream file("files/leaderboard.txt");
                board.mine_count = 0;
                content_str = board.contentstr(leaderboard, mins * 60 + secs, name);
                file << content_str;
                file.close();
            }
            if (debug && !won) { board.mine_debug(debug, game); }
            if (won) { game.draw(win_sprite); }
            if (lost) { game.draw(lose_sprite); }
            game.display();
        }
    }
};

#endif //MINESWEEPER_GAMEWINDOW_H
