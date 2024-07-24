#ifndef MINESWEEPER_BOARD_H
#define MINESWEEPER_BOARD_H
#include <vector>
#include <SFML/Window.hpp>
#include <SFML/Graphics.hpp>
#include "Tile.h"
#include <sstream>
#include <iomanip>
#include <string>
using namespace std;

class Board {
public:
    int col_count;
    int row_count;
    int mine_count;
    vector<vector<Tile>> board_vect;
    bool leaderOn = false;
    // empty board
    Board(int col_count, int row_count, int mine_count){
        this->col_count = col_count;
        this->row_count = row_count;
        this->mine_count = mine_count;
    }
    // creates a board with default parameters
    void blankBoard(){
        board_vect.clear();
        for (int i = 0; i < row_count; i++) {
            vector<Tile> temp;
            for (int j = 0; j < col_count; j++) {
                Tile tile = Tile();
                tile.hidden_tile_texture.loadFromFile("files/images/tile_hidden.png");
                // i = horiz. position (x-coord) // j = vert. position (y-coord)
                tile.hidden_tile.setPosition(j*32, i*32);
                temp.push_back(tile);
            }
            board_vect.push_back(temp);
            temp.clear();
        }
    }
    // main function for handling what tiles get revealed
    bool reveal(int x, int y, sf::RenderWindow& game) {
        int row = y / 32;
        int col = x / 32;
        float cell_center_x = col * 32;
        float cell_center_y = row * 32;

        Tile& reveal_tile = board_vect[row][col];
        if (reveal_tile.is_revealed || reveal_tile.is_flag) {
            return true;
        }
        reveal_tile.revealed_tile_texture.loadFromFile("files/images/tile_revealed.png");
        reveal_tile.revealed_tile.setPosition(cell_center_x, cell_center_y);
        reveal_tile.revealed_tile.setTexture(reveal_tile.revealed_tile_texture);
        reveal_tile.tile_rev();
        if (reveal_tile.is_mine) {
            return false;
        } else if (reveal_tile.adjacent_mines == 0) {
            for (int i = 0; i<reveal_tile.adj_tiles.size(); i++) {
                if (!reveal_tile.adj_tiles.at(i)->is_mine && !reveal_tile.adj_tiles.at(i)->is_revealed && !reveal_tile.adj_tiles.at(i)->is_flag) {
                    reveal(reveal_tile.adj_tiles.at(i)->hidden_tile.getPosition().x, reveal_tile.adj_tiles.at(i)->hidden_tile.getPosition().y, game);
                }
            }
        } else {
            reveal_tile.setTextures(reveal_tile.adjMinesStr());
            reveal_tile.num_sprite.setPosition(cell_center_x, cell_center_y);
        }
        return true;
    }
    // when the debug button is pressed/player clicks a mine,
    // all the mines show up on the board
    void mine_debug(bool debug, sf::RenderWindow& game) {
        sf::Texture mine_texture;
        mine_texture.loadFromFile("files/images/mine.png");
        sf::Sprite mine_sprite(mine_texture);
        for (int i = 0; i < row_count; i++) {
            for (int j = 0; j < col_count; j++) {
                if (debug && board_vect[i][j].is_mine) {
                    mine_sprite.setPosition(j * 32, i * 32);
                    game.draw(mine_sprite);
                } else if (!debug && !board_vect[i][j].is_revealed) {
                    board_vect[i][j].hidden_tile.setPosition(j * 32, i * 32);
                    game.draw(board_vect[i][j].hidden_tile);
                }
            }
        }
    }
    // randomizes mines and places them on board
    void makeMines() {
        random_device rd;
        mt19937 gen(rd());
        uniform_int_distribution<int> dis_row(0, row_count - 1);
        uniform_int_distribution<int> dis_col(0, col_count - 1);
        int mines_placed = 0;
        while (mines_placed < mine_count) {
            int row = dis_row(gen);
            int col = dis_col(gen);
            if (!board_vect[row][col].is_mine) {
                board_vect[row][col].is_mine = true;
                mines_placed++;
            }
        }
        setAdjTiles();
    }
    // updates the vector of tiles adjacent to each tile
    void setAdjTiles() {
        // create vectors of the adjacent tiles
        for (int i = 0; i < row_count; i++) {
            for (int j = 0; j < col_count; j++) {
                Tile& current_tile = board_vect[i][j];
                for (int di = -1; di <= 1; di++) {
                    for (int dj = -1; dj <= 1; dj++) {
                        int ni = i + di;
                        int nj = j + dj;
                        if (ni >= 0 && ni < row_count && nj >= 0 && nj < col_count && !(di == 0 && dj == 0)) {
                            current_tile.adj_tiles.push_back(&board_vect[ni][nj]);
                        }
                    }
                }
            }
        }
        // update tiles "adj mines" number
        for (int i = 0; i < row_count; i++) {
            for (int j = 0; j < col_count; j++) {
                int num = 0;
                for (int tiles = 0; tiles < board_vect[i][j].adj_tiles.size(); tiles++) {
                    if (board_vect[i][j].adj_tiles.at(tiles)->is_mine) {num++;}
                }
                board_vect[i][j].adjacent_mines = num;
            }
        }
    }
    // handles flags being placed and unplaced
    void toggleFlag(int x, int y) {
        int row = y / 32;
        int col = x / 32;
        float cell_center_x = col * 32;
        float cell_center_y = row * 32;
        board_vect[row][col].flag_texture.loadFromFile("files/images/flag.png");
        board_vect[row][col].is_flag = !board_vect[row][col].is_flag;
        if (board_vect[row][col].is_flag) {
            board_vect[row][col].flag_sprite.setTexture(board_vect[row][col].flag_texture);
            board_vect[row][col].flag_sprite.setPosition(cell_center_x, cell_center_y);
            mine_count--;
        } else {
            board_vect[row][col].flag_sprite.setTexture(board_vect[row][col].hidden_tile_texture);
            mine_count++;
        }
    }
    bool checkWin() {
        for (int i = 0; i < row_count; i++) {
            for (int j = 0; j < col_count; j++) {
                if (!board_vect[i][j].is_revealed && !board_vect[i][j].is_mine) {
                    return false;
                }
            }
        }
        for (int i = 0; i < row_count; i++) {
            for (int j = 0; j < col_count; j++) {
                if (board_vect[i][j].is_mine) {
                    board_vect[i][j].is_flag = true;
                    board_vect[i][j].flag_texture.loadFromFile("files/images/flag.png");
                    board_vect[i][j].flag_sprite.setTexture(board_vect[i][j].flag_texture);
                    board_vect[i][j].flag_sprite.setPosition(j * 32, i * 32);
                }
            }
        }
        return true;
    }
    void drawDigs(sf::RenderWindow& game) {
        sf::Texture digits_texture;
        digits_texture.loadFromFile("files/images/digits.png");
        sf::Sprite hundreds_digits_sprite(digits_texture);
        sf::Sprite tens_digits_sprite(digits_texture);
        sf::Sprite ones_digits_sprite(digits_texture);
        sf::Sprite neg_digits_sprite(digits_texture);

        int abs_mine_count = abs(mine_count);

        sf::IntRect hundreds_texture_rect(21*(floor(abs_mine_count/100)), 0, 21, 32);
        hundreds_digits_sprite.setTextureRect(hundreds_texture_rect);
        sf::IntRect tens_texture_rect(21*(floor(abs_mine_count/10)), 0, 21, 32);
        tens_digits_sprite.setTextureRect(tens_texture_rect);
        sf::IntRect ones_texture_rect(21*(floor(abs_mine_count%10)), 0, 21, 32);
        ones_digits_sprite.setTextureRect(ones_texture_rect);
        sf::IntRect neg_texture_rect(21*10, 0, 21, 32);
        neg_digits_sprite.setTextureRect(neg_texture_rect);

        neg_digits_sprite.setPosition(12, 32*(row_count+0.5)+16);
        hundreds_digits_sprite.setPosition(33, 32*(row_count+0.5)+16);
        tens_digits_sprite.setPosition(33+21, 32*(row_count+0.5)+16);
        ones_digits_sprite.setPosition(33+21+21, 32*(row_count+0.5)+16);

        game.draw(hundreds_digits_sprite);
        game.draw(tens_digits_sprite);
        game.draw(ones_digits_sprite);
        if (mine_count<0) {game.draw(neg_digits_sprite);}
    }
    void drawClock(sf::RenderWindow& game, int mins, int secs) {
        sf::Texture digits_texture;
        digits_texture.loadFromFile("files/images/digits.png");
        sf::Sprite tens_mins_sprite(digits_texture);
        sf::Sprite ones_mins_sprite(digits_texture);
        sf::Sprite tens_secs_sprite(digits_texture);
        sf::Sprite ones_secs_sprite(digits_texture);

        sf::IntRect tens_mins_texture_rect(21*(floor(mins/10)), 0, 21, 32);
        tens_mins_sprite.setTextureRect(tens_mins_texture_rect);
        sf::IntRect ones_mins_texture_rect(21*(floor(mins%10)), 0, 21, 32);
        ones_mins_sprite.setTextureRect(ones_mins_texture_rect);
        sf::IntRect tens_secs_texture_rect(21*(floor(secs/10)), 0, 21, 32);
        tens_secs_sprite.setTextureRect(tens_secs_texture_rect);
        sf::IntRect ones_secs_texture_rect(21*(floor(secs%10)), 0, 21, 32);
        ones_secs_sprite.setTextureRect(ones_secs_texture_rect);

        tens_mins_sprite.setPosition(col_count*32-97, 32*(row_count+0.5)+16);
        ones_mins_sprite.setPosition(col_count*32-97+21, 32*(row_count+0.5)+16);
        tens_secs_sprite.setPosition(col_count*32-54, 32*(row_count+0.5)+16);
        ones_secs_sprite.setPosition(col_count*32-54+21, 32*(row_count+0.5)+16);

        game.draw(tens_mins_sprite);
        game.draw(ones_mins_sprite);
        game.draw(tens_secs_sprite);
        game.draw(ones_secs_sprite);
    }
    void loadLeaderboard(const string& filename, map<int, string> &leaderboard) {
        ifstream file(filename);
        string line;
        int count = 0;
        while (getline(file, line) && count < 5) {
            istringstream iss(line);
            string timeStr, player_name, numStr;
            if (getline(iss, numStr, '\t') && getline(iss, timeStr, '\t') && getline(iss, player_name)) {
                // Check if timeStr has at least the minimum length for expected format
                if (timeStr.size() >= 5) {
                    int time_secs = stoi(timeStr.substr(0, 2)) * 60 + stoi(timeStr.substr(3));
                    leaderboard[time_secs] = player_name;
                }
                getline(iss, numStr);
            }
            count++;
        }
        file.close();
    }
    string contentstr(map<int, string> &mymap, int time, string name) {
        string info = "";
        int rank = 1;
        int count = 0;
        for (auto iter = mymap.begin(); iter != mymap.end(); iter++) {
            if (count == 5) {break;}
            info += to_string(rank) + ".\t";
            int mins = iter->first / 60;
            int secs = iter->first % 60;
            stringstream time_ss;
            time_ss << setw(2) << setfill('0') << mins << ":" << setw(2) << setfill('0') << secs;
            string time_str = time_ss.str();
            string trimmed_name = iter->second;
            trimmed_name.erase(0, trimmed_name.find_first_not_of(" "));
            // Check if the trimmed name matches the input name and time
            if (trimmed_name == name && iter->first == time) {
                trimmed_name += "*";
            } else if (trimmed_name.find("*") != string::npos) {
                trimmed_name = trimmed_name.substr(0, trimmed_name.length()-1);
            }
            info += time_str + " \t" + trimmed_name + "\n\n";
            rank++;
            count++;
        }
        return info;
    }
};

#endif //MINESWEEPER_BOARD_H

