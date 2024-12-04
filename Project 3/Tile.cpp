#include "Tile.h"
using namespace std;

Tile::Tile(sf::Vector2f position, string tileSprite) {
    this->position = position;
    this->tileSprite = sf::Sprite(TextureManager::GetTexture(tileSprite));
    this->tileSprite.setPosition(position);
    hasFlag = false;
    hasMine = false;
    hidden = true;
}

sf::Sprite& Tile::GetTileSprite() {
    return tileSprite;
}

sf::Sprite& Tile::GetTopSprite() {
    return topSprite;
}

int Tile::GetAdjacentTileCount() {
    return adjacentTiles.size();
}

Tile* Tile::GetAdjacentTile(int tile) {
    return adjacentTiles.at(tile);
}

bool Tile::CheckMine() const {
    return hasMine;
}

bool Tile::CheckFlag() const {
    return hasFlag;
}

bool Tile::CheckHidden() const {
    return hidden;
}

void Tile::SetMine(bool set) {
    if(set)
        hasMine = true;
    else
        hasMine = false;
}

void Tile::RevealTile() {
    hidden = false;
}

void Tile::SetTileSprite(const char* texture) {
    tileSprite.setTexture(TextureManager::GetTexture(texture));
    tileSprite.setPosition(position);
}

void Tile::SetTopSprite(const char* texture) {
    topSprite.setTexture(TextureManager::GetTexture(texture));
    topSprite.setPosition(position);
}

void Tile::AddAdjacentTile(Tile* tile) {
    adjacentTiles.push_back(tile);
}


