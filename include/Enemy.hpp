#pragma once
#include "Animator.hpp"
#include "Entity.hpp"
#include "Pickup.hpp"
#include <memory>
#include <vector>

class Player;
class AcidBlob;

struct AcidTile {
    sf::Vector2f position;
    float        ttl;
    bool         active{ true };
};

class Enemy : public Entity {
public:
    Enemy(sf::Vector2f position, int hp, float speed, int damage);
    virtual ~Enemy() = default;

    void update(float dt) override;

    virtual void pushBack(sf::Vector2f force) = 0;

    virtual void move(const Player& target, float dt) = 0;
    virtual void attack(Player& target, float dt)     = 0;
    virtual std::unique_ptr<Pickup> onDeath();

    // All enemies use shape_ — override once here for every subclass
    sf::FloatRect getBounds() const override { return shape_.getGlobalBounds(); }

    void  takeDamage(int amount);
    int   getHealth()    const { return health_; }
    int   getMaxHealth() const { return maxHealth_; }
    float getSpeed()     const { return speed_; }
    bool  isDead()       const { return health_ <= 0; }
    void  applySpeedMultiplier(float m) { speedMult_ = m; }

protected:
    int   health_;
    int   maxHealth_;
    float speed_;
    float speedMult_{ 1.f };
    int   damage_;
    float attackTimer_{ 0.f };
    
    sf::CircleShape shape_;
};

// ── BeetleBot ────────────────────────────────────────────────────────────────
class BeetleBot : public Enemy {
private: 
    sf::Sprite sprite_;
    Animator animator_;

public:
    explicit BeetleBot(sf::Vector2f position);
    void pushBack(sf::Vector2f force) override {
        position_ += force;
        sprite_.setPosition(position_);
    }
    void update(float dt) override; //updating the animator
    void draw(sf::RenderWindow& window) override;
    void move(const Player& target, float dt) override;
    void attack(Player& target, float dt)     override;
    sf::FloatRect getBounds() const override { return sprite_.getGlobalBounds(); }
};

// ── MosquitoDrone ────────────────────────────────────────────────────────────
class MosquitoDrone : public Enemy {
public:
    explicit MosquitoDrone(sf::Vector2f position);

    void update(float dt) override;
    void draw(sf::RenderWindow& window) override;
    void move(const Player& target, float dt) override;
    void attack(Player& target, float dt)     override;
    std::unique_ptr<AcidBlob> tryShoot(const Player& target);

    void pushBack(sf::Vector2f force) override {
        position_ += force;
        sprite_.setPosition(position_);
    }

    sf::FloatRect getBounds() const override { return sprite_.getGlobalBounds();}

private:
    sf::Sprite sprite_;
    Animator animator_;

    float orbitAngle_{ 0.f };
    float fireTimer_ { 0.f };
    bool isShooting_{ false }; //tracks animations
};

// ── SludgeCrawler ────────────────────────────────────────────────────────────
class SludgeCrawler : public Enemy {
public:
    explicit SludgeCrawler(sf::Vector2f position);
    
    void update(float dt) override;
    void draw(sf::RenderWindow& window) override;
    void move(const Player& target, float dt) override;
    void attack(Player& target, float dt)     override;
    std::unique_ptr<Pickup> onDeath() override;
    AcidTile makeTrailTile() const;

    void pushBack(sf::Vector2f force) override {
        position_ += force;
        sprite_.setPosition(position_);
    }

    sf::FloatRect getBounds() const override { return sprite_.getGlobalBounds(); }

private:
    sf::Sprite sprite_;
    Animator animator_;
    float trailTimer_{ 0.f };
};

// ── ScorpionMech (boss) ──────────────────────────────────────────────────────
class ScorpionMech : public Enemy {
public:
    explicit ScorpionMech(sf::Vector2f position);

    void update(float dt) override;
    void draw(sf::RenderWindow& window) override;
    void move(const Player& target, float dt) override;
    void attack(Player& target, float dt)     override;
    int  getPhase() const { return phase_; }
    std::unique_ptr<AcidBlob> tryShoot(const Player& target);

    void pushBack(sf::Vector2f force) override {
        position_ += force;
        sprite_.setPosition(position_);
    }

    sf::FloatRect getBounds() const override { return sprite_.getGlobalBounds(); }

private:
    void  checkPhaseSwitch();

    sf::Sprite sprite_;
    Animator animator_;

    int   phase_      { 1 };
    float fireTimer_  { 0.f };
    float chargeTimer_{ 0.f };
    bool isShooting_  { false }; //tracks cannon fire anim 
};
