#include "Enemy.hpp"
#include "AssetManager.hpp"
#include "Player.hpp"
#include "Projectile.hpp"
#include "Constants.hpp"
#include <cmath>
#include <algorithm>
#include <cstdlib>

// ─────────────────────────────────────────────────────────────────────────────
//  Enemy base
// ─────────────────────────────────────────────────────────────────────────────
Enemy::Enemy(sf::Vector2f position, int hp, float speed, int damage)
    : Entity(position)
    , health_(hp), maxHealth_(hp), speed_(speed), damage_(damage)
{}

void Enemy::update(float dt) {
    // Subclasses call move() and attack() themselves inside their own update
    // or via Game; we just keep the base no-op so Game can call entity->update()
}

void Enemy::takeDamage(int amount) {
    health_ = std::max(0, health_ - amount);
    if (health_ == 0) alive_ = false;
}

std::unique_ptr<Pickup> Enemy::onDeath() {
    return std::make_unique<PowerCell>(position_);
}

// ─────────────────────────────────────────────────────────────────────────────
//  Helper  –  normalised vector toward target
// ─────────────────────────────────────────────────────────────────────────────
static sf::Vector2f towardTarget(sf::Vector2f from, sf::Vector2f to) {
    sf::Vector2f d = to - from;
    float len = std::hypot(d.x, d.y);
    if (len < 0.001f) return {};
    return d / len;
}

// ─────────────────────────────────────────────────────────────────────────────
//  BeetleBot  –  melee seek
// ─────────────────────────────────────────────────────────────────────────────
BeetleBot::BeetleBot(sf::Vector2f position)
    : Enemy(position, VS::BEETLE_HP, VS::BEETLE_SPEED, VS::BEETLE_DAMAGE)
    , sprite_(AssetManager::getInstance().getTexture("beetle"))
    , animator_(sprite_) //initialize animator
{      
    sprite_.setOrigin({ 16.f, 16.f });
    sprite_.setScale({ 2.0f, 2.0f });
    sprite_.setPosition(position_);

    animator_.addAnimation("Walk", 32, 32, 1, 0, 1.0f);
    animator_.play("Walk");
}

void BeetleBot::update(float dt) {
    Enemy::update(dt);
    animator_.update(dt);
}

void BeetleBot::move(const Player& target, float dt) {
    sf::Vector2f dir = towardTarget(position_, target.getPosition());
    position_ += dir * speed_ * speedMult_ * dt;
   
    if(dir.x < 0.f) { 
        sprite_.setScale({ -2.0f, 2.0f }); //face left
    } else if(dir.x > 0.f) {
        sprite_.setScale({ 2.0f, 2.0f });  //face right
    }

    sprite_.setPosition(position_);
}

void BeetleBot::attack(Player& target, float dt) {
    float dist = std::hypot(position_.x - target.getPosition().x,
                             position_.y - target.getPosition().y);
    attackTimer_ += dt;

    if (dist < 32.f + 36.f && attackTimer_ >= 0.5f) {  
        target.takeDamage(10);
        attackTimer_ = 0.f;
    }
}

void BeetleBot::draw(sf::RenderWindow& window) {
    if (alive_) window.draw(sprite_);
}

// ─────────────────────────────────────────────────────────────────────────────
//  MosquitoDrone  –  orbit + ranged shots
// ─────────────────────────────────────────────────────────────────────────────
MosquitoDrone::MosquitoDrone(sf::Vector2f position)
    : Enemy(position, VS::MOSQUITO_HP, VS::MOSQUITO_SPEED, VS::MOSQUITO_DAMAGE)
    , sprite_(AssetManager::getInstance().getTexture("mosquito"))
    , animator_(sprite_)

{
    sprite_.setOrigin({ 16.f, 16.f });
    sprite_.setScale({ 1.5f, 1.5f });
    sprite_.setPosition(position);

    animator_.addAnimation("Fly", 32, 32, 1, 0, 1.0f);
    animator_.addAnimation("Shoot", 32, 32, 1, 0, 1.0f);
    animator_.play("Fly"); 

    orbitAngle_ = static_cast<float>(std::rand() % 628) / 100.f;
}

void MosquitoDrone::update(float dt) {
    Enemy::update(dt);
    animator_.update(dt);
}

void MosquitoDrone::move(const Player& target, float dt) {
    float angularSpeed = (speed_ * speedMult_) / VS::MOSQUITO_ORBIT_RADIUS;
    orbitAngle_ += angularSpeed * dt;

    sf::Vector2f tp = target.getPosition();
    position_.x = tp.x + VS::MOSQUITO_ORBIT_RADIUS * std::cos(orbitAngle_);
    position_.y = tp.y + VS::MOSQUITO_ORBIT_RADIUS * std::sin(orbitAngle_);
    
    sf::Vector2f dir = towardTarget(position_, tp);
    if(dir.x < 0.f) sprite_.setScale({ -1.5f, 1.5f });
    else if (dir.x > 0.f) sprite_.setScale({ 1.5f, 1.5f });

    sprite_.setPosition(position_);
}

void MosquitoDrone::attack(Player& /*target*/, float dt) {
    fireTimer_ += dt;   // shooting handled via tryShoot() in Game

    float cooldown = 1.f / VS::MOSQUITO_FIRE_RATE;
    if (fireTimer_ >= cooldown - 0.3f && !isShooting_) {
        animator_.play("Shoot");
        isShooting_ = true;
    }
    else if (fireTimer_ < cooldown - 0.3f && isShooting_) {
        animator_.play("Fly");
        isShooting_ = false;
    }
}

std::unique_ptr<AcidBlob> MosquitoDrone::tryShoot(const Player& target) {
    float cooldown = 1.f / VS::MOSQUITO_FIRE_RATE;
    
    if (fireTimer_ < cooldown) return nullptr;
    
    fireTimer_ = 0.f;
    isShooting_ = false;
    animator_.play("Fly");

    sf::Vector2f dir = towardTarget(position_, target.getPosition());
    return std::make_unique<AcidBlob>(position_, dir);
}

void MosquitoDrone::draw(sf::RenderWindow& window) {
    if (alive_) window.draw(sprite_);
}

// ─────────────────────────────────────────────────────────────────────────────
//  SludgeCrawler  –  slow; leaves acid trail; drops RepairKit
// ─────────────────────────────────────────────────────────────────────────────
SludgeCrawler::SludgeCrawler(sf::Vector2f position)
    : Enemy(position, VS::SLUDGE_HP, VS::SLUDGE_SPEED, VS::SLUDGE_DAMAGE)
    , sprite_(AssetManager::getInstance().getTexture("sludge"))
    , animator_(sprite_)

{
    sprite_.setOrigin({ 16.f, 16.f });
    sprite_.setScale({ 2.0f, 2.0f });
    sprite_.setPosition(position);

    animator_.addAnimation("crawl", 32, 32, 1, 0, 1.0f);
    animator_.play("crawl");
}

void SludgeCrawler::update(float dt) {
    Enemy::update(dt);
    animator_.update(dt);
}

void SludgeCrawler::move(const Player& target, float dt) {
    sf::Vector2f dir = towardTarget(position_, target.getPosition());
    position_ += dir * speed_ * speedMult_ * dt;

    if(dir.x < 0.f) sprite_.setScale({ -2.0f, 2.0f });
    else if (dir.x > 0.f) sprite_.setScale({ 2.0f, 2.0f });

    sprite_.setPosition(position_);
    trailTimer_ += dt;
}

void SludgeCrawler::attack(Player& target, float dt) {
    float dist = std::hypot(position_.x - target.getPosition().x,
                             position_.y - target.getPosition().y);
    attackTimer_ += dt;
    if (dist < 32.f + 36.f && attackTimer_ >= 0.5f) { 
        target.takeDamage(15);
        attackTimer_ = 0.f;
    }
}

AcidTile SludgeCrawler::makeTrailTile() const {
    return { position_, VS::SLUDGE_TRAIL_TTL, true };
}

std::unique_ptr<Pickup> SludgeCrawler::onDeath() {
    return std::make_unique<RepairKit>(position_);
}

void SludgeCrawler::draw(sf::RenderWindow& window) {
    if (alive_) window.draw(sprite_);
}

// ─────────────────────────────────────────────────────────────────────────────
//  ScorpionMech  –  boss
// ─────────────────────────────────────────────────────────────────────────────
ScorpionMech::ScorpionMech(sf::Vector2f position)
    : Enemy(position, VS::BOSS_HP, VS::BOSS_SPEED, VS::BOSS_DAMAGE)
    , sprite_(AssetManager::getInstance().getTexture("scorpion"))
    , animator_(sprite_)
{
    sprite_.setTexture(AssetManager::getInstance().getTexture("scorpion"));
    sprite_.setOrigin({ 64.f, 64.f });
    sprite_.setPosition(position);
    sprite_.setScale({ 1.5f, 1.5f });
    
    animator_.addAnimation("Phase1_Walk", 128, 128, 1, 0, 1.0f);
    animator_.addAnimation("Phase1_Shoot", 128, 128, 1, 0, 1.0f);
    animator_.addAnimation("Phase2_Charge", 128, 128, 1, 0, 1.0f);

    animator_.play("Phase1_Walk");
}

void ScorpionMech::update(float dt) {
    Enemy::update(dt);
    animator_.update(dt);
}

void ScorpionMech::checkPhaseSwitch() {
    float ratio = static_cast<float>(health_) / static_cast<float>(maxHealth_);
    if (phase_ == 1 && ratio <= VS::BOSS_PHASE2_THRESH) {
        phase_ = 2;
        // if 50% hp or below switch to phase 2 anims
        animator_.play("Phase2_Charge");
        sprite_.setColor(sf::Color{ 200, 50, 255 }); //tint purple to indicate enraged phase
    }
}

void ScorpionMech::move(const Player& target, float dt) {
    checkPhaseSwitch();
    
    sf::Vector2f dir = towardTarget(position_, target.getPosition());

    float dist = std::hypot(position_.x - target.getPosition().x,
                            position_.y - target.getPosition().y);

    if (phase_ == 1) {
        if (!isShooting_) {
            if (dist > 300.f)
                position_ += dir * speed_ * speedMult_ * dt;
            else if (dist < 200.f)
                position_ -= dir * speed_ * speedMult_ * dt;
        }
    } else {
        if (dist > 90.f) {
            position_ += dir * speed_ * speedMult_ * 1.5f * dt;
        }
    }

    if (dir.x < 0.f) sprite_.setScale({ -1.5f, 1.5f }); 
    else if (dir.x > 0.f) sprite_.setScale({ 1.5f, 1.5f });

    sprite_.setPosition(position_);
}

void ScorpionMech::attack(Player& target, float dt) {
    if (phase_ == 2) {
        float dist = std::hypot(position_.x - target.getPosition().x,
                                 position_.y - target.getPosition().y);
        attackTimer_ += dt;
        if (dist < 96.f + 36.f && attackTimer_ >= 1.0f) {
            target.takeDamage(20);
            attackTimer_ = 0.f;
        }
    } else {
        fireTimer_ += dt; //pre shooting animation handled
        float cooldown = 2.0f; // shoots every 2 seconds in phase 1
            if (fireTimer_ >= cooldown - 0.3f && !isShooting_){ 
                animator_.play("Phase1_Shoot");
                isShooting_ = true;
        }
    }
    
}

std::unique_ptr<AcidBlob> ScorpionMech::tryShoot(const Player& target) {
    if (phase_ == 1 && isShooting_ && fireTimer_ >= 2.0f) {

    fireTimer_ = 0.f; 
    isShooting_ = false;

    animator_.play("Phase1_Walk"); //go back to walking anim

    sf::Vector2f dir = towardTarget(position_, target.getPosition());
    return std::make_unique<AcidBlob>(position_, dir);
    }
    return nullptr;
}
void ScorpionMech::draw(sf::RenderWindow& window) {
    if (alive_) window.draw(sprite_);
}
