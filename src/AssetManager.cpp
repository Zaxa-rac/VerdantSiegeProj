#include "AssetManager.hpp"
#include <iostream>

bool AssetManager::loadTexture(const std::string& name, const std::string& filepath) {
    sf::Texture texture;
    if (!texture.loadFromFile(filepath)) {
        std::cerr << "Failed to load texture: " << filepath << "\n";
        return false;
    }
    textures_[name] = std::move(texture);
    return true;
}

const sf::Texture& AssetManager::getTexture(const std::string& name) const {
    auto it = textures_.find(name);
    if (it != textures_.end()) {
        return it->second;
    }
    throw std::runtime_error("Texture not found: " + name);
}
