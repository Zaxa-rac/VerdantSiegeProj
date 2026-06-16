#pragma once
#include <SFML/Graphics.hpp>

// ─────────────────────────────────────────────────────────────────────────────
//  Entity  –  abstract base for every game object.

class Entity {
public:
    explicit Entity(sf::Vector2f position = {0.f, 0.f});
    virtual ~Entity() = default;

    virtual void update(float dt) = 0;
    virtual void draw(sf::RenderWindow& window) = 0;

    // Subclasses override this to return their sprites bounds
    virtual sf::FloatRect getBounds() const {
        return { {position_.x - 10.f, position_.y - 10.f}, {20.f, 20.f} };
    }

    bool         isAlive()    const { return alive_; }
    sf::Vector2f getPosition()const { return position_; }
    void         setPosition(sf::Vector2f pos) { position_ = pos; }
    void         kill() { alive_ = false; }

protected:
    sf::Vector2f position_;
    bool         alive_{ true };
};
