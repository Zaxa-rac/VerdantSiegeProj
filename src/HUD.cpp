#include "HUD.hpp"
#include "Player.hpp"
#include "Constants.hpp"
#include <cstdint>
#include <string>

HUD::HUD() {
    hpBarBg_.setSize({ 200.f, 18.f });
    hpBarBg_.setPosition({ 10.f, 10.f });
    hpBarBg_.setFillColor(VS::COLOR_HP_BG);

    hpBarFill_.setSize({ 200.f, 18.f });
    hpBarFill_.setPosition({ 10.f, 10.f });
    hpBarFill_.setFillColor(VS::COLOR_HP_BAR);

    for (int i = 0; i < MAX_PIPS; ++i) {
        barrierPips_[i].setSize({ 24.f, 14.f });
        barrierPips_[i].setPosition({ 10.f + i * 30.f, 34.f });
        barrierPips_[i].setFillColor(sf::Color{ 50, 200, 100 });
    }
}

bool HUD::loadFont(const std::string& fontPath) {
    fontLoaded_ = font_.openFromFile(fontPath);
    if (fontLoaded_) {
        // Construct the optional sf::Text objects now that we have a font
        scoreText_   = sf::Text(font_, "SCORE: 0",   20);
        waveText_    = sf::Text(font_, "WAVE 1",     20);
        enemiesText_ = sf::Text(font_, "ENEMIES: 0", 16);

        for (auto* t : { &*scoreText_, &*waveText_, &*enemiesText_ })
            t->setFillColor(sf::Color::White);

        scoreText_->setPosition(  { VS::WINDOW_W / 2.f - 60.f, 8.f });
        waveText_->setPosition(   { VS::WINDOW_W - 180.f,       8.f });
        enemiesText_->setPosition({ VS::WINDOW_W - 220.f,      32.f });
    }
    return fontLoaded_;
}

void HUD::update(const Player& player, int waveNumber, int enemiesLeft) {
    float ratio = static_cast<float>(player.getHealth()) /
                  static_cast<float>(player.getMaxHealth());
    hpBarFill_.setSize({ 200.f * ratio, 18.f });

    int charges = player.getBarrierCharges();
    for (int i = 0; i < MAX_PIPS; ++i) {
        barrierPips_[i].setFillColor(
            i < charges ? sf::Color{ 50, 200, 100 } : sf::Color{ 60, 60, 60 });
    }

    if (fontLoaded_) {
        scoreText_->setString(  "SCORE: "   + std::to_string(player.getScore()));
        waveText_->setString(   "WAVE "     + std::to_string(waveNumber));
        enemiesText_->setString("ENEMIES: " + std::to_string(enemiesLeft));
    }
}

void HUD::draw(sf::RenderWindow& window) const {
    window.draw(hpBarBg_);
    window.draw(hpBarFill_);
    for (const auto& pip : barrierPips_) window.draw(pip);
    if (fontLoaded_) {
        window.draw(*scoreText_);
        window.draw(*waveText_);
        window.draw(*enemiesText_);
    }
}
