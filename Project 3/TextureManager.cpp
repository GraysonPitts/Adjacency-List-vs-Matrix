#include "TextureManager.h"
using namespace std;

unordered_map<string, sf::Texture> TextureManager::textures;

void TextureManager::LoadTexture(const string& fileName) {
    string filePath = "files/images/";
    filePath += fileName;
    filePath += ".png";

    textures[fileName].loadFromFile(filePath);
}

sf::Texture& TextureManager::GetTexture(const string& key) {
    if (key == "null") {
        sf::Texture temp;
        return temp;
    }
    if (textures.find(key) == textures.end()) {
        LoadTexture(key);
    }

    return textures[key];
}