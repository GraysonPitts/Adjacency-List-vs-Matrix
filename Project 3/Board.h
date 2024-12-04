#pragma once
#include "Tile.h"
#include "SFML/Graphics.hpp"
#include "TextureManager.h"
#include <vector>
using namespace std;

class Board {
private:
    int mineCount = 0;
    int flagCount = 0;
    unsigned int tileWidth;
    unsigned int tileHeight;
    bool debugMode = false;
    bool paused = false;
    bool gameOver = false;
    bool victory = false;
    vector<Tile> board;
public:
    Board(int mineCount, unsigned int tileWidth, unsigned int tileHeight);

    Tile& GetTile(int tile);
    unsigned int GetSize() const;
    int GetFlagCount() const;
    bool CheckPaused() const;
    bool CheckGameOver() const;
    bool CheckVictory();

    void ToggleDebug();
    void TogglePause();
    void SetVictory(bool yes_no);
    void SetGameOver(bool yes_no);

    void Initialize();
    void SetAllNeighbors();
    void Reveal(Tile* tile);

};

