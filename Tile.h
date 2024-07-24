#ifndef MINESWEEPER_TILE_H
#define MINESWEEPER_TILE_H
#include <vector>
#include <SFML/Window.hpp>
#include <SFML/Graphics.hpp>
using namespace std;


class Tile {
public:
    bool is_revealed;
    bool is_mine;
    bool is_flag;
    vector<Tile*> adj_tiles;
    int adjacent_mines;
    sf::Sprite hidden_tile;
    sf::Texture hidden_tile_texture;
    sf::Sprite revealed_tile;
    sf::Texture revealed_tile_texture;
    sf::Sprite flag_sprite;
    sf::Texture flag_texture;
    sf::Sprite cover_sprite;
    sf::Texture cover_texture;
    sf::Sprite blank;

    sf::Texture number_texture;
    sf::Sprite num_sprite;


    Tile() {
        is_revealed = false;
        is_mine = false;
        is_flag = false;
    }
    void tile_rev() {
        is_revealed = true;
    }
    string adjMinesStr() {
        return to_string(adjacent_mines);
    }
    void setTextures(string num) {
        string file_str = "files/images/number_" + num + ".png";
        number_texture.loadFromFile(file_str);
        num_sprite.setTexture(number_texture);
    }
};


#endif //MINESWEEPER_TILE_H
