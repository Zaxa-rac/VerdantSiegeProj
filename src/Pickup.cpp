#include "Pickup.hpp"
#include "Player.hpp"
#include "Constants.hpp"

// ── Pickup base ───────────────────────────────────────────────────────────────
Pickup::Pickup(sf::Vector2f position, int value)
    : Entity(position), value_(value)
{
    shape_.setRadius(VS::PICKUP_RADIUS);
    shape_.setOrigin({ VS::PICKUP_RADIUS, VS::PICKUP_RADIUS });
    shape_.setPosition(position);
}

// NOTE: update() is defined inline in Pickup.hpp — do NOT add it here again.

// ── PowerCell ─────────────────────────────────────────────────────────────────
PowerCell::PowerCell(sf::Vector2f position)
    : Pickup(position, VS::POWER_CELL_VALUE)
{
    shape_.setFillColor(sf::Color{ 255, 220, 30 });
}
void PowerCell::draw(sf::RenderWindow& window) {
    if (alive_ && !collected_) window.draw(shape_);
}
void PowerCell::onCollect(Player& player) {
    player.addScore(value_);
    collected_ = true;
    alive_     = false;
}

// ── RepairKit ─────────────────────────────────────────────────────────────────
RepairKit::RepairKit(sf::Vector2f position)
    : Pickup(position, VS::REPAIR_KIT_HP)
{
    shape_.setFillColor(sf::Color{ 80, 220, 80 });
}
void RepairKit::draw(sf::RenderWindow& window) {
    if (alive_ && !collected_) window.draw(shape_);
}
void RepairKit::onCollect(Player& player) {
    player.heal(value_);
    collected_ = true;
    alive_     = false;
}

// ── FertiliserCanister ────────────────────────────────────────────────────────
FertiliserCanister::FertiliserCanister(sf::Vector2f position)
    : Pickup(position, 0)
{
    shape_.setFillColor(sf::Color{ 200, 130, 50 });
}
void FertiliserCanister::draw(sf::RenderWindow& window) {
    if (alive_ && !collected_) window.draw(shape_);
}
void FertiliserCanister::onCollect(Player& player) {
    player.activateFertiliser();
    collected_ = true;
    alive_     = false;
}
