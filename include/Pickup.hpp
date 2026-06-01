#pragma once
#include "Entity.hpp"

class Player;

class Pickup : public Entity {
public:
    Pickup(sf::Vector2f position, int value);
    virtual ~Pickup() = default;

    void update(float /*dt*/) override {}
    virtual void onCollect(Player& player) = 0;

    sf::FloatRect getBounds() const override { return shape_.getGlobalBounds(); }

    bool isCollected() const { return collected_; }
    int  getValue()    const { return value_; }

protected:
    int             value_;
    bool            collected_{ false };
    sf::CircleShape shape_;
};

class PowerCell : public Pickup {
public:
    explicit PowerCell(sf::Vector2f position);
    void draw(sf::RenderWindow& window) override;
    void onCollect(Player& player) override;
};

class RepairKit : public Pickup {
public:
    explicit RepairKit(sf::Vector2f position);
    void draw(sf::RenderWindow& window) override;
    void onCollect(Player& player) override;
};

class FertiliserCanister : public Pickup {
public:
    explicit FertiliserCanister(sf::Vector2f position);
    void draw(sf::RenderWindow& window) override;
    void onCollect(Player& player) override;
};
