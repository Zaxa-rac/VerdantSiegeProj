#pragma once
#include <SFML/Graphics.hpp>
#include <unordered_map>
#include <string>
#include <stdexcept>

class AssetManager {
public:
    static AssetManager& getInstance() {
        static AssetManager instance;
        return instance;
    }

    bool loadTexture(const std::string& name, const std::string& filepath); 
    const sf::Texture& getTexture(const std::string& name) const;

private:
    AssetManager() = default;
    std::unordered_map<std::string, sf::Texture> textures_;
};