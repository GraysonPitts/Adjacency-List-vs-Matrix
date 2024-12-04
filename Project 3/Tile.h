#pragma once
#include "SFML/Graphics.hpp"
#include "TextureManager.h"
#include <vector>
using namespace std;

class Tile {
private:
    bool hasFlag;
    bool hasMine;
    bool hidden = true;
    sf::Sprite tileSprite;
    sf::Sprite topSprite;
    sf::Vector2f position;
    vector<Tile*> adjacentTiles;
public:
    Tile(sf::Vector2f position, string sprite);

    sf::Sprite& GetTileSprite();
    sf::Sprite& GetTopSprite();
    int GetAdjacentTileCount();
    Tile* GetAdjacentTile(int tile);
    bool CheckMine() const;
    bool CheckFlag() const;
    bool CheckHidden() const;

    void SetMine(bool set);
    void RevealTile();
    void SetTileSprite(const char* texture);
    void SetTopSprite(const char* texture);
    void AddAdjacentTile(Tile* tile);


};



