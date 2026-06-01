#pragma once
#include "Entity.hpp"

class Projectile : public Entity {
public:
    enum class Owner { Player, Enemy };

    Projectile(sf::Vector2f position, sf::Vector2f velocity,
               int damage, float range, Owner owner);
    virtual ~Projectile() = default;

    void update(float dt) override;
    virtual void onHit(Entity& target);
    virtual bool isBarrier() const { return false; }
    sf::FloatRect getBounds() const override { return shape_.getGlobalBounds(); }

    Owner getOwner()  const { return owner_; }
    int   getDamage() const { return damage_; }
    bool  hasExpired() const;

protected:
    sf::Vector2f  velocity_;
    int           damage_;
    float         range_;
    float         travelled_{ 0.f };
    Owner         owner_;
    sf::CircleShape shape_;
};

class SporeShot : public Projectile {
public:
    SporeShot(sf::Vector2f origin, sf::Vector2f direction);
    void draw(sf::RenderWindow& window) override;
    void onHit(Entity& target) override;
};

class AcidBlob : public Projectile {
public:
    AcidBlob(sf::Vector2f origin, sf::Vector2f direction);
    void draw(sf::RenderWindow& window) override;
    void onHit(Entity& target) override;
};
