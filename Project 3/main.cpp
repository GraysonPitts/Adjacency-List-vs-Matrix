#include "TextureManager.h"
#include "Tile.h"
#include "Board.h"
#include <iostream>
#include <SFML/Graphics.hpp>
#include <fstream>
#include <sstream>
#include <vector>
#include <chrono>
using namespace std;


void setText(sf::Text &text, float x, float y) {
    sf::FloatRect textRect = text.getLocalBounds();
    text.setOrigin(textRect.left + textRect.width/2.0f,
                   textRect.top + textRect.height/2.0f);
    text.setPosition(sf::Vector2f(x, y));
}

vector<int> splitIntoDigits(int number) {
    vector<int> numbers;

    // Minimum of -99
    if (number < -99) {
        numbers.push_back(10);
        numbers.push_back(9);
        numbers.push_back(9);
    }
    // -99 to -10
    else if (number < -9) {
        numbers.push_back(10);
        int temp = (number * -1) / 10;
        numbers.push_back(temp);
        temp = (number * -1) % 10;
        numbers.push_back(temp);
    }
    // -9 to -1
    else if (number < 0) {
        numbers.push_back(10);
        numbers.push_back(0);
        numbers.push_back(number * -1);
    }
    // 0 to 9
    else if (number < 10) {
        numbers.push_back(0);
        numbers.push_back(0);
        numbers.push_back(number);
    }
    // 10 to 99
    else if (number < 100) {
        numbers.push_back(0);
        int temp = number / 10;
        numbers.push_back(temp);
        temp = number % 10;
        numbers.push_back(temp);
    }
    // 100 to 999
    else if (number < 1000) {
        int temp = number / 100;
        numbers.push_back(temp);
        temp = number % 100;
        temp /= 10;
        numbers.push_back(temp);
        temp = number % 10;
        numbers.push_back(temp);
    }
    // Max of 999
    else {
        numbers.push_back(9);
        numbers.push_back(9);
        numbers.push_back(9);
    }

    return numbers;
}

struct Leader {
    string time;
    string name;
};

void openLeaderboard(int columnCount, int rowCount, int elapsed_minutes, int elapsed_seconds) {
    int windowWidth = columnCount * 16;
    int windowHeight = (rowCount * 16) + 50;
    vector<Leader> leaders;
    string time;
    string name;

    fstream leaderFile("files/leaderboard.txt");
    if (!leaderFile.is_open()) {
        cout << "Couldn't find config file!" << endl;
        return;
    }

    std::string line;
    while (getline(leaderFile, line, ',')) {
        cout << line << endl;
        istringstream iss(line);
        Leader leader;
        char comma;
        char number;
        iss >> number >> leader.time >> leader.name;
        leaders.push_back(leader);
    }

    sf::Font font;
    font.loadFromFile("files/font.ttf");

    sf::RenderWindow LeaderWindow(sf::VideoMode(windowWidth, windowHeight), "Minesweeper", sf::Style::Close);

    sf::Text leaderText("LEADERBOARD", font, 20);
    leaderText.setStyle(sf::Text::Bold | sf::Text::Underlined);
    leaderText.setFillColor(sf::Color::White);
    setText(leaderText, (float)windowWidth / 2, ((float)windowHeight / 2) - 120);


    while(LeaderWindow.isOpen()) {
        sf::Event event{};
        while(LeaderWindow.pollEvent(event)) {
            if(event.type == sf::Event::Closed) {
                LeaderWindow.close();
            }
        }

        LeaderWindow.clear(sf::Color::Blue);
        LeaderWindow.draw(leaderText);
        LeaderWindow.display();
    }
}

void startGame(int columnCount, int rowCount, int mineCount) {
    int windowWidth = columnCount * 32;
    int windowHeight = (rowCount * 32) + 100;
    auto startTime = std::chrono::high_resolution_clock::now();
    auto endTime = std::chrono::high_resolution_clock::now();
    auto elapsed_seconds = std::chrono::duration_cast<std::chrono::seconds>(endTime - startTime).count();
    auto elapsed_minutes = std::chrono::duration_cast<std::chrono::minutes>(endTime - startTime).count();
    int pauseSeconds = 0;
    int pauseMinutes = 0;
    bool leaderBoardActive = false;

    sf::Font font;
    font.loadFromFile("files/font.ttf");

    sf::RenderWindow GameWindow(sf::VideoMode(windowWidth, windowHeight), "Minesweeper", sf::Style::Close);

    // Create Board and Tiles
    Board board = Board(mineCount, columnCount, rowCount);
    board.Initialize();

    // Generate UI Textures/Sprites
    sf::Sprite happyFace(TextureManager::GetTexture("face_happy"));
    happyFace.setPosition(((columnCount/2.0)*32.0)-32, 32*(rowCount+0.5));

    sf::Sprite debugButton(TextureManager::GetTexture("debug"));
    debugButton.setPosition((columnCount*32.0)-304, 32*(rowCount+0.5));

    sf::Sprite pauseButton(TextureManager::GetTexture("pause"));
    pauseButton.setPosition((columnCount*32.0)-240, 32*(rowCount+0.5));
    TextureManager::GetTexture("play"); // Preloading play button texture for later

    sf::Sprite leaderButton(TextureManager::GetTexture("leaderboard"));
    leaderButton.setPosition((columnCount*32.0)-176, 32*(rowCount+0.5));

    sf::Texture digitTexture = TextureManager::GetTexture("digits");
    vector<sf::IntRect> digitRects;
    for (int i = 0; i < 211; i += 21) {
        digitRects.emplace_back(i, 0, 21, 32);
    }

    // Counter Generation
    vector<int> MineNums = splitIntoDigits(mineCount);
    vector<sf::Sprite> counterDisplay;
    for (int i = 0, j = 33; i < 3; i++) {
        counterDisplay.emplace_back(digitTexture);
        counterDisplay.at(i).setPosition(j, (32 * (rowCount + 0.5) + 16));
        j += 21;
        counterDisplay.at(i).setTextureRect(digitRects.at(MineNums.at(i)));
    }

    while (GameWindow.isOpen()) {
        if (!board.CheckPaused()) {
            endTime = std::chrono::high_resolution_clock::now();
            elapsed_seconds = std::chrono::duration_cast<std::chrono::seconds>(endTime - startTime).count() - pauseSeconds;
            elapsed_minutes = std::chrono::duration_cast<std::chrono::minutes>(endTime - startTime).count() - pauseMinutes;
        }
        else {
            auto pauseTime = std::chrono::high_resolution_clock::now();
            pauseSeconds = std::chrono::duration_cast<std::chrono::seconds>(pauseTime - endTime).count();
            pauseMinutes = std::chrono::duration_cast<std::chrono::minutes>(pauseTime - endTime).count();
        }


        // Timer Generation
        while (elapsed_seconds >= 60) {
            elapsed_seconds -= 60;
        }
        vector<int> SecondNums = splitIntoDigits(elapsed_seconds);
        vector<sf::Sprite> timerSecondsDisplay;
        for (int i = 0, j = ((columnCount * 32) - 75); i < 3; i++) {
            timerSecondsDisplay.emplace_back(digitTexture);
            timerSecondsDisplay.at(i).setPosition(j, (32 * (rowCount + 0.5) + 16));
            j += 21;
            timerSecondsDisplay.at(i).setTextureRect(digitRects.at(SecondNums.at(i)));
        }
        vector<int> MinuteNums = splitIntoDigits(elapsed_minutes);
        vector<sf::Sprite> timerMinutesDisplay;
        for (int i = 0, j = ((columnCount * 32) - 118); i < 3; i++) {
            timerMinutesDisplay.emplace_back(digitTexture);
            timerMinutesDisplay.at(i).setPosition(j, (32 * (rowCount + 0.5) + 16));
            j += 21;
            timerMinutesDisplay.at(i).setTextureRect(digitRects.at(MinuteNums.at(i)));
        }

        sf::Event event{};
        while (GameWindow.pollEvent(event)) {
            if (event.type == sf::Event::Closed) {
                GameWindow.close();
            }
            else if (event.type == sf::Event::MouseButtonPressed) {

                // Left Mouse Button
                if (event.mouseButton.button == sf::Mouse::Left) {
                    sf::Vector2i position = sf::Mouse::getPosition(GameWindow);

                    // Pause Button
                    if ((position.x > pauseButton.getPosition().x &&
                         position.x < pauseButton.getPosition().x + pauseButton.getTextureRect().width)
                        && (position.y > pauseButton.getPosition().y &&
                            position.y < pauseButton.getPosition().y + pauseButton.getTextureRect().height) &&
                            !board.CheckGameOver() && !board.CheckVictory() && !leaderBoardActive) {
                        board.TogglePause();
                        if (board.CheckPaused()) {
                            pauseButton.setTexture(TextureManager::GetTexture("play"));
                            pauseSeconds = 0;
                            pauseMinutes = 0;
                        }
                        else
                            pauseButton.setTexture(TextureManager::GetTexture("pause"));
                    }

                    // Leaderboard button
                    if ((position.x > leaderButton.getPosition().x &&
                         position.x < leaderButton.getPosition().x + leaderButton.getTextureRect().width)
                        && (position.y > leaderButton.getPosition().y &&
                            position.y < leaderButton.getPosition().y + leaderButton.getTextureRect().height)) {
                        leaderBoardActive = true;
                        board.TogglePause();
                        if (board.CheckPaused()) {
                            pauseButton.setTexture(TextureManager::GetTexture("play"));
                            pauseSeconds = 0;
                            pauseMinutes = 0;
                        }
                        else
                            pauseButton.setTexture(TextureManager::GetTexture("pause"));
                    }

                    // Won't work if game is paused
                    if (!board.CheckPaused()) {

                        // Happy Face Button (Start New Game)
                        if ((position.x > happyFace.getPosition().x &&
                             position.x < happyFace.getPosition().x + happyFace.getTextureRect().width)
                            && (position.y > happyFace.getPosition().y &&
                                position.y < happyFace.getPosition().y + happyFace.getTextureRect().height)) {
                            board.Initialize();
                            happyFace.setTexture(TextureManager::GetTexture("face_happy"));
                            MineNums = splitIntoDigits(mineCount - board.GetFlagCount());
                            for (int i = 0; i < 3; i++)
                                counterDisplay.at(i).setTextureRect(digitRects.at(MineNums.at(i)));
                            startTime = std::chrono::high_resolution_clock::now();
                        }

                        // Debug Button Toggle
                        if ((position.x > debugButton.getPosition().x &&
                             position.x < debugButton.getPosition().x + debugButton.getTextureRect().width)
                            && (position.y > debugButton.getPosition().y &&
                                position.y < debugButton.getPosition().y + debugButton.getTextureRect().height)) {
                            board.ToggleDebug();
                        }

                        // Reveal Tile
                        if (position.y < windowHeight-65 && !board.CheckGameOver() && !board.CheckVictory()) {
                            Tile* currentSpot = &board.GetTile(25 * (position.y / 32) + (position.x / 32));
                            board.Reveal(currentSpot);
                            if (board.CheckVictory()) {
                                happyFace.setTexture(TextureManager::GetTexture("face_win"));
                                for (int i = 0; i < 3; i++) {
                                    MineNums.at(i) = 0;
                                    counterDisplay.at(i).setTextureRect(digitRects.at(MineNums.at(i)));
                                }
                            }
                            if (board.CheckGameOver())
                                happyFace.setTexture(TextureManager::GetTexture("face_lose"));
                        }
                    }
                }
            }
        }

        GameWindow.clear(sf::Color::White);
        GameWindow.draw(happyFace);
        GameWindow.draw(debugButton);
        GameWindow.draw(pauseButton);
        GameWindow.draw(leaderButton);
        for (int i = 0; i < 3; i++)
            GameWindow.draw(counterDisplay.at(i));

        for (int i = 1; i < 3; i++)
            GameWindow.draw(timerSecondsDisplay.at(i));
        for (int i = 1; i < 3; i++)
            GameWindow.draw(timerMinutesDisplay.at(i));

        for (unsigned int i = 0; i < board.GetSize(); i++) {
            GameWindow.draw(board.GetTile(i).GetTileSprite());
            GameWindow.draw(board.GetTile(i).GetTopSprite());
        }

        GameWindow.display();

        if (leaderBoardActive) {
            openLeaderboard(columnCount, rowCount, elapsed_minutes, elapsed_seconds);
            leaderBoardActive = false;
            board.TogglePause();
        }
    }
}

int main() {
    ifstream configFile("files/config.cfg");
    if (!configFile.is_open()) {
        cout << "Couldn't find config file!" << endl;
        return 0;
    }

    int columnCount;
    int rowCount;
    int mineCount;
    configFile >> columnCount >> rowCount >> mineCount;
    int windowWidth = columnCount * 32;
    int windowHeight = (rowCount * 32) + 100;

    sf::Font font;
    font.loadFromFile("files/font.ttf");

    sf::RenderWindow WelcomeWindow(sf::VideoMode(windowWidth, windowHeight), "Minesweeper", sf::Style::Close);

    sf::Text welcomeText("WELCOME TO MINESWEEPER!", font, 24);
    welcomeText.setStyle(sf::Text::Bold | sf::Text::Underlined);
    welcomeText.setFillColor(sf::Color::White);
    setText(welcomeText, (float)windowWidth / 2, ((float)windowHeight / 2) - 150);

    sf::Text nameText("Enter your name:", font, 20);
    nameText.setStyle(sf::Text::Bold);
    nameText.setFillColor(sf::Color::White);
    setText(nameText, (float)windowWidth / 2, ((float)windowHeight / 2) - 75);

    sf::String userInput = '|';  // String that will update with user data in the loop
    sf::Text userText(userInput, font, 18);
    userText.setStyle(sf::Text::Bold);
    userText.setFillColor(sf::Color::Yellow);
    setText(userText, (float)windowWidth / 2, ((float)windowHeight / 2) - 45);


    while(WelcomeWindow.isOpen()) {
        sf::Event event{};
        while(WelcomeWindow.pollEvent(event)) {
            if(event.type == sf::Event::Closed) {
                WelcomeWindow.close();
            }

            if(event.type == sf::Event::TextEntered and isalpha(static_cast<int>(event.text.unicode)) and userInput.getSize() < 11) {
                userInput.erase(userInput.getSize() - 1);
                unsigned char temp = event.text.unicode;

                if (userInput.getSize() == 0) {
                    temp = toupper(temp);
                }
                else {
                    temp = tolower(temp);
                }

                userInput += static_cast<char>(temp);
                userText.setString(userInput += '|');
                setText(userText, (float)windowWidth / 2, ((float)windowHeight / 2) - 45);
            }
            else if (event.type == sf::Event::KeyPressed and userInput.getSize() > 1) {
                if (event.key.code == sf::Keyboard::BackSpace) {
                    userInput.erase(userInput.getSize() - 2);
                    userText.setString(userInput);
                    setText(userText, (float)windowWidth / 2, ((float)windowHeight / 2) - 45);
                }
                else if (event.key.code == sf::Keyboard::Enter) {
                    WelcomeWindow.close();
                    startGame(columnCount, rowCount, mineCount);
                }

            }
        }

        WelcomeWindow.clear(sf::Color::Blue);
        WelcomeWindow.draw(welcomeText);
        WelcomeWindow.draw(nameText);
        WelcomeWindow.draw(userText);
        WelcomeWindow.display();
    }
    return 0;
}
