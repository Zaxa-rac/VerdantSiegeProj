#pragma once
#include "Entity.hpp"
#include "Animator.hpp"
#include "Projectile.hpp"
#include "ThornBarrier.hpp"
#include "Constants.hpp"
#include <vector>
#include <memory>

class Player : public Entity {
public:
    Player();

    void update(float dt) override;
    void draw(sf::RenderWindow& window) override;

    sf::FloatRect getBounds() const override { return sprite_.getGlobalBounds(); }

    void handleInput(const sf::RenderWindow& window, float dt);

    std::unique_ptr<SporeShot>    shoot(sf::Vector2f targetPos);
    std::unique_ptr<ThornBarrier> placeBarrier(sf::Vector2f pos);

    int  getHealth()         const { return health_; }
    int  getMaxHealth()      const { return VS::PLAYER_MAX_HP; }
    int  getBarrierCharges() const { return barrierCharges_; }
    int  getScore()          const { return score_; }
    bool isDead()            const { return health_ <= 0; }
    bool isFertilised()      const { return fertiliserTimer_ > 0.f; }

    void takeDamage(int amount);
    void heal(int amount);
    void addScore(int points);
    void activateFertiliser();
    void applySpeedMultiplier(float m, float duration);

    bool         shootRequested_  { false };
    bool         barrierRequested_{ false };
    sf::Vector2f mouseWorldPos_;

private:
    int   health_         { VS::PLAYER_MAX_HP };
    int   barrierCharges_ { VS::PLAYER_MAX_BARRIERS };
    int   score_          { 0 };

    float shootCooldown_    { 0.f };
    float barrierRegenTimer_{ 0.f };
    float fertiliserTimer_  { 0.f };
    float speedMultTimer_   { 0.f };
    float speedMult_        { 1.f };
    float baseFireRate_     { VS::PLAYER_SHOOT_COOLDOWN };

    sf::Sprite sprite_;
    Animator   animator_;
};
