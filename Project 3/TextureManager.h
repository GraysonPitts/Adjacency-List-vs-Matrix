#pragma once
#include "SFML/Graphics.hpp"
#include <iostream>
#include <unordered_map>
#include <string>
using namespace std;

class TextureManager {
private:
    static unordered_map<string, sf::Texture> textures;
    static void LoadTexture(const string& fileName);
public:
    static sf::Texture& GetTexture(const string& key);
};