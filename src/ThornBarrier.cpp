#include "ThornBarrier.hpp"
#include "Constants.hpp"
#include <cstdint>
#include <algorithm>

ThornBarrier::ThornBarrier(sf::Vector2f center)
    : Projectile(center, {0.f, 0.f}, VS::BARRIER_DPS, 0.f, Owner::Player)
    , lifetime_(VS::BARRIER_LIFETIME)
{
    rect_.setSize({ VS::BARRIER_W, VS::BARRIER_H });
    rect_.setOrigin({ VS::BARRIER_W / 2.f, VS::BARRIER_H / 2.f });
    rect_.setPosition(center);
    rect_.setFillColor(VS::COLOR_BARRIER_TINT);
    rect_.setOutlineColor(sf::Color{ 50, 200, 80 });
    rect_.setOutlineThickness(2.f);
}
void ThornBarrier::update(float dt) {
    if (!alive_) return;
    lifetime_ -= dt;
    if (lifetime_ <= 0.f) { 
        alive_ = false; 
        return; 
    }

    float alpha = std::max(0.f, lifetime_ / VS::BARRIER_LIFETIME);
    auto c = rect_.getFillColor();
    c.a = static_cast<std::uint8_t>(180.f * alpha);  // std::uint8_t
    rect_.setFillColor(c);
}
void ThornBarrier::draw(sf::RenderWindow& window) {
    if (alive_) window.draw(rect_);
}