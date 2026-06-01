#pragma once
#include <SFML/Graphics.hpp>
#include <optional>
#include <string>

class Player;

// sf::Text can't be default-constructed in SFML 3 (needs a font).
// We use std::optional so the members start empty and are created
// only once the font is successfully loaded.
class HUD {
public:
    HUD();

    bool loadFont(const std::string& fontPath);
    void update(const Player& player, int waveNumber, int enemiesLeft);
    void draw(sf::RenderWindow& window) const;

private:
    sf::Font font_;
    bool     fontLoaded_{ false };

    sf::RectangleShape hpBarBg_;
    sf::RectangleShape hpBarFill_;

    static constexpr int MAX_PIPS = 3;
    sf::RectangleShape   barrierPips_[MAX_PIPS];

    // std::optional — empty until loadFont() succeeds
    std::optional<sf::Text> scoreText_;
    std::optional<sf::Text> waveText_;
    std::optional<sf::Text> enemiesText_;
};
