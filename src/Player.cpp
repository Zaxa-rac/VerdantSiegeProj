#include "Player.hpp"
#include "AssetManager.hpp"
#include "Constants.hpp"
#include <cmath>
#include <algorithm>

Player::Player()
    : Entity({ VS::ARENA_W / 2.f, VS::ARENA_H / 2.f })
    , sprite_(AssetManager::getInstance().getTexture("player"))
    , animator_(sprite_)
{
    sprite_.setOrigin({ 16.f, 16.f });
    sprite_.setScale({ 2.0f, 2.0f });
    sprite_.setPosition(position_);

    animator_.addAnimation("Idle", 32, 32, 1, 0, 1.0f);
    animator_.addAnimation("Run", 32, 32, 1, 0, 1.0f);

    animator_.play("Idle");
}

void Player::update(float dt) {
    if (shootCooldown_   > 0.f) shootCooldown_   -= dt;
    if (fertiliserTimer_ > 0.f) fertiliserTimer_ -= dt;

    if (barrierCharges_ < VS::PLAYER_MAX_BARRIERS) {
        barrierRegenTimer_ += dt;
        if (barrierRegenTimer_ >= VS::PLAYER_BARRIER_REGEN) {
            barrierRegenTimer_ = 0.f;
            ++barrierCharges_;
        }
    }

    if (speedMultTimer_ > 0.f) {
        speedMultTimer_ -= dt;
        if (speedMultTimer_ <= 0.f) speedMult_ = 1.f;
    }
    animator_.update(dt);
}

void Player::handleInput(const sf::RenderWindow& window, float dt) {
    sf::Vector2f dir{ 0.f, 0.f };

    if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::W)) dir.y -= 1.f;
    if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::S)) dir.y += 1.f;
    if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::A)) dir.x -= 1.f;
    if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::D)) dir.x += 1.f;

    float len = std::hypot(dir.x, dir.y);
    if (len > 0.f) {
        dir /= len;
        animator_.play("Run"); //switching to run anim
    
        if(dir.x < 0.f) sprite_.setScale({ -2.0f, 2.0f }); // face left
        if(dir.x > 0.f) sprite_.setScale({ 2.0f, 2.0f }); // face right
    }else{
        animator_.play("Idle");
    } 

    float effectiveSpeed = VS::PLAYER_SPEED * speedMult_;
    position_ += dir * effectiveSpeed * dt;

    position_.x = std::clamp(position_.x, VS::PLAYER_RADIUS, VS::ARENA_W - VS::PLAYER_RADIUS);
    position_.y = std::clamp(position_.y, VS::PLAYER_RADIUS, VS::ARENA_H - VS::PLAYER_RADIUS);
    sprite_.setPosition(position_);

    mouseWorldPos_ = window.mapPixelToCoords(sf::Mouse::getPosition(window));
}

std::unique_ptr<SporeShot> Player::shoot(sf::Vector2f targetPos) {
    if (shootCooldown_ > 0.f) return nullptr;

    float cooldown = isFertilised()
        ? baseFireRate_ / VS::FERTILISER_FIRE_MULT
        : baseFireRate_;
    shootCooldown_ = cooldown;

    sf::Vector2f dir = targetPos - position_;
    float len = std::hypot(dir.x, dir.y);
    if (len < 0.001f) return nullptr;
    dir /= len;

    return std::make_unique<SporeShot>(position_, dir);
}

std::unique_ptr<ThornBarrier> Player::placeBarrier(sf::Vector2f pos) {
    if (barrierCharges_ <= 0) return nullptr;
    --barrierCharges_;
    return std::make_unique<ThornBarrier>(pos);
}

void Player::takeDamage(int amount) {
    health_ = std::max(0, health_ - amount);
    if (health_ == 0) alive_ = false;
}

void Player::heal(int amount)          { health_ = std::min(VS::PLAYER_MAX_HP, health_ + amount); }
void Player::addScore(int points)      { score_ += points; }
void Player::activateFertiliser()      { fertiliserTimer_ = VS::FERTILISER_DURATION; }

void Player::applySpeedMultiplier(float m, float duration) {
    speedMult_     = m;
    speedMultTimer_ = duration;
}

void Player::draw(sf::RenderWindow& window) {
    window.draw(sprite_); 
}
