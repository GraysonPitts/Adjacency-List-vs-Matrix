#include "Board.h"
#include <random>
#include <chrono>
using namespace std;

Board::Board(int mineCount, unsigned int tileWidth, unsigned int tileHeight) {
    this->mineCount = mineCount;
    this->tileWidth = tileWidth;
    this->tileHeight = tileHeight;
}

Tile& Board::GetTile(int tile) {
    return board.at(tile);
}

unsigned int Board::GetSize() const {
    return tileWidth * tileHeight;
}

int Board::GetFlagCount() const {
    return flagCount;
}

bool Board::CheckPaused() const {
    return paused;
}

bool Board::CheckGameOver() const {
    return gameOver;
}

bool Board::CheckVictory() {
    int count = 0;
    for (int i = 0; i < GetSize(); i++) {
        if (!board.at(i).CheckHidden()) {
            count++;
        }
    }
    if (GetSize() - count - mineCount == 0) {
        for (auto & i : board) {
            if (i.CheckMine()) {
                i.SetTopSprite("flag");
            }
        }
        victory = true;
        return true;
    }
    return false;
}

void Board::ToggleDebug() {
    if (debugMode)
        debugMode = false;
    else
        debugMode = true;
    if (!gameOver && !victory) {
        for (int i = 0; i < GetSize(); i++) {
            if (debugMode && board.at(i).CheckMine())
                board.at(i).SetTopSprite("mine");
            else if (!debugMode && board.at(i).CheckFlag())
                board.at(i).SetTopSprite("flag");
            else if (!debugMode && board.at(i).CheckMine()) {
                board.at(i).SetTopSprite("null");
            }
        }
    }
}

void Board::TogglePause() {
    if (paused)
        paused = false;
    else
        paused = true;

    if (!gameOver && !victory) {
        for (int i = 0; i < GetSize(); i++) {
            if (paused) {
                board.at(i).SetTopSprite("tile_revealed");
            }
            else if (board.at(i).CheckFlag()) {
                board.at(i).SetTileSprite("tile_hidden");
                board.at(i).SetTopSprite("flag");
            }
            else if (board.at(i).CheckMine()) {
                board.at(i).SetTileSprite("tile_hidden");
                board.at(i).SetTopSprite("mine");
            }
            else {
                board.at(i).SetTopSprite("null");
            }
        }
    }

}

void Board::SetVictory(bool yes_no) {
    if (yes_no)
        victory = true;
    else
        victory = false;
}

void Board::SetGameOver(bool yes_no) {
    if (yes_no) {
        gameOver = true;
        for (auto & i : board) {
            if (i.CheckMine()) {
                i.SetTopSprite("mine");
            }
        }
    }
    else
        gameOver = false;
}

void Board::Initialize() {

    auto startTime = std::chrono::high_resolution_clock::now();
    auto elapsed_time = std::chrono::duration_cast<std::chrono::milliseconds>(startTime.time_since_epoch());


    // seed with time, passed parameter omitted
    std::mt19937 foo(elapsed_time.count());
    std::uniform_int_distribution<> random(0, 399);

    // Generate random mine positions
    vector<int> randoMines;
    randoMines.push_back(random(foo));
    while (randoMines.size() < mineCount) {
        int x = random(foo);
        bool dupe = false;
        for (int randoMine : randoMines) {
            if (x == randoMine) {
                dupe = true;
                break;
            }
        }
        if (!dupe)
            randoMines.push_back(x);
    }

    // Initialize Board
    gameOver = false;
    victory = false;
    board.clear();
    mineCount = 0;
    flagCount = 0;

    // Generate empty tiles
    for (int i = 0; i < tileHeight; i++) {
        for (int j = 0; j < tileWidth; j++) {
            sf::Vector2f position = sf::Vector2f((float)(j) * 32, (float)(i) * 32);
            Tile temp = Tile(position, "tile_hidden");
            board.push_back(temp);
        }
    }

    // Set Bombs based on random list
    for (int randoMine : randoMines) {
        if (randoMine >= 0 && randoMine < board.size()) {
            board.at(randoMine).SetMine(true);
            mineCount++;
            if (debugMode) {
                board.at(randoMine).SetTopSprite("mine");
            }
        }
    }

    SetAllNeighbors();
}

void Board::SetAllNeighbors() {
    for (int i = 0; i < board.size(); i++) {
        // Top Left
        if (i == 0) {
            board.at(i).AddAdjacentTile(&board.at(1));
            board.at(i).AddAdjacentTile(&board.at(tileWidth));
            board.at(i).AddAdjacentTile(&board.at(tileWidth + 1));
        }
        // Top Right
        else if (i == tileWidth - 1) {
            board.at(i).AddAdjacentTile(&board.at(i - 2));
            board.at(i).AddAdjacentTile(&board.at(i * 2));
            board.at(i).AddAdjacentTile(&board.at((i * 2) + 1));
        }
        // Bottom Left
        else if (i == (tileWidth * tileHeight) - tileWidth) {
            board.at(i).AddAdjacentTile(&board.at(i - tileWidth));
            board.at(i).AddAdjacentTile(&board.at(i - tileWidth + 1));
            board.at(i).AddAdjacentTile(&board.at(i + 1));
        }
        // Bottom Right
        else if (i == (tileWidth * tileHeight) - 1) {
            board.at(i).AddAdjacentTile(&board.at(i - tileWidth));
            board.at(i).AddAdjacentTile(&board.at(i - tileWidth - 1));
            board.at(i).AddAdjacentTile(&board.at(i - 1));
        }

        // Top Edge
        else if (i > 0 && i < tileWidth - 1) {
            board.at(i).AddAdjacentTile(&board.at(i - 1));
            board.at(i).AddAdjacentTile(&board.at(i + 1));
            board.at(i).AddAdjacentTile(&board.at(i + tileWidth - 1));
            board.at(i).AddAdjacentTile(&board.at(i + tileWidth));
            board.at(i).AddAdjacentTile(&board.at(i + tileWidth + 1));
        }
        // Bottom Edge
        else if (i > (tileWidth * tileHeight) - tileWidth && i < (tileWidth * tileHeight) - 1) {
            board.at(i).AddAdjacentTile(&board.at(i - tileWidth - 1));
            board.at(i).AddAdjacentTile(&board.at(i - tileWidth));
            board.at(i).AddAdjacentTile(&board.at(i - tileWidth + 1));
            board.at(i).AddAdjacentTile(&board.at(i - 1));
            board.at(i).AddAdjacentTile(&board.at(i + 1));
        }
        // Left Edge
        else if (i % tileWidth == 0) {
            board.at(i).AddAdjacentTile(&board.at(i + 1));
            board.at(i).AddAdjacentTile(&board.at(i + 1 - tileWidth));
            board.at(i).AddAdjacentTile(&board.at(i + 1 + tileWidth));
            board.at(i).AddAdjacentTile(&board.at(i + tileWidth));
            board.at(i).AddAdjacentTile(&board.at(i - tileWidth));
        }
        // Right Edge
        else if ((i + 1) % tileWidth == 0) {
            board.at(i).AddAdjacentTile(&board.at(i - 1));
            board.at(i).AddAdjacentTile(&board.at(i - tileWidth));
            board.at(i).AddAdjacentTile(&board.at(i + tileWidth));
            board.at(i).AddAdjacentTile(&board.at(i - 1 - tileWidth));
            board.at(i).AddAdjacentTile(&board.at(i - 1 + tileWidth));
        }
        // Anywhere else
        else {
            board.at(i).AddAdjacentTile(&board.at(i - 1));
            board.at(i).AddAdjacentTile(&board.at(i - 1 + tileWidth));
            board.at(i).AddAdjacentTile(&board.at(i - 1 - tileWidth));
            board.at(i).AddAdjacentTile(&board.at(i + 1));
            board.at(i).AddAdjacentTile(&board.at(i + 1 + tileWidth));
            board.at(i).AddAdjacentTile(&board.at(i + 1 - tileWidth));
            board.at(i).AddAdjacentTile(&board.at(i + tileWidth));
            board.at(i).AddAdjacentTile(&board.at(i - tileWidth));
        }
    }
}

void Board::Reveal(Tile* tile) {
    if (!tile->CheckHidden()) {
        return;
    }
    if (tile->CheckMine()) {
        SetGameOver(true);
        return;
    }

    int count = 0;
    // Check for adjacent mines in order to assign number
    for (int i = 0; i < tile->GetAdjacentTileCount(); i++) {
        if (tile->GetAdjacentTile(i)->CheckMine()) {
            count++;
        }
    }
    // Set numbers
    if (count > 0) {
        string spriteName = "number_" + to_string(count);
        tile->SetTileSprite(spriteName.c_str());
        tile->RevealTile();
        return;
    }
    else {
        tile->SetTileSprite("tile_revealed");
        tile->RevealTile();
        for (int i = 0; i < tile->GetAdjacentTileCount(); i++) {
            Tile* temp = tile->GetAdjacentTile(i);
            if (temp->CheckHidden() && !temp->CheckFlag() && !temp->CheckMine()) {
                Reveal(temp);
            }
        }
    }
}