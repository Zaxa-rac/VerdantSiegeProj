#pragma once
#include "Projectile.hpp"
#include <SFML/Graphics.hpp>


class ThornBarrier : public Projectile {
public:
    explicit ThornBarrier(sf::Vector2f center);

    void update(float dt) override;
    
    void draw(sf::RenderWindow& window) override;
    
    bool isBarrier() const override { return true; }

    sf::FloatRect getBounds() const override { return rect_.getGlobalBounds(); }

private:
    sf::RectangleShape rect_;

    float lifetime_{ 4.0f }; 
    float age_{ 0.f };       
};