#include "Projectile.hpp"
#include "Constants.hpp"
#include <cmath>

// ─────────────────────────────────────────────────────────────────────────────
//  Projectile base
// ─────────────────────────────────────────────────────────────────────────────
Projectile::Projectile(sf::Vector2f position,
                       sf::Vector2f velocity,
                       int          damage,
                       float        range,
                       Owner        owner)
    : Entity(position)
    , velocity_(velocity)
    , damage_(damage)
    , range_(range)
    , owner_(owner)
{}

void Projectile::update(float dt) {
    if (!alive_) return;
    sf::Vector2f delta = velocity_ * dt;
    position_  += delta;
    shape_.setPosition(position_);
    travelled_ += std::hypot(delta.x, delta.y);
    if (hasExpired()) alive_ = false;
}

void Projectile::onHit(Entity& /*target*/) {
    alive_ = false;   // default: destroy on contact
}

bool Projectile::hasExpired() const {
    return !alive_ || travelled_ >= range_;
}

// ─────────────────────────────────────────────────────────────────────────────
//  SporeShot
// ─────────────────────────────────────────────────────────────────────────────
SporeShot::SporeShot(sf::Vector2f origin, sf::Vector2f direction)
    : Projectile(origin, direction * VS::SPORE_SPEED,
                 VS::SPORE_DAMAGE, VS::SPORE_RANGE,
                 Owner::Player)
{
    shape_.setRadius(VS::SPORE_RADIUS);
    shape_.setOrigin({VS::SPORE_RADIUS, VS::SPORE_RADIUS});
    shape_.setFillColor(sf::Color{ 80, 220, 60 });
    shape_.setPosition(origin);
}

void SporeShot::draw(sf::RenderWindow& window) {
    if (alive_) window.draw(shape_);
}

void SporeShot::onHit(Entity& target) {
    // Particle emission is handled by Game after this returns
    alive_ = false;
    (void)target;
}

// ─────────────────────────────────────────────────────────────────────────────
//  AcidBlob
// ─────────────────────────────────────────────────────────────────────────────
AcidBlob::AcidBlob(sf::Vector2f origin, sf::Vector2f direction)
    : Projectile(origin, direction * 300.f,
                 10, 600.f,
                 Owner::Enemy)
{
    shape_.setRadius(7.f);
    shape_.setOrigin({7.f, 7.f});
    shape_.setFillColor(sf::Color{ 255, 30, 150, 220 });
    shape_.setPosition(origin);
}

void AcidBlob::draw(sf::RenderWindow& window) {
    if (alive_) window.draw(shape_);
}

void AcidBlob::onHit(Entity& target) {
    alive_ = false;
    (void)target;
}
