#include "ParticleSystem.hpp"
#include <cstdint>
#include <cmath>
#include <random>
#include <algorithm>

static std::mt19937 rng{ std::random_device{}() };

sf::Vector2f ParticleSystem::randomVelocity(float minSpeed, float maxSpeed) {
    std::uniform_real_distribution<float> angleDist(0.f, 2.f * 3.14159f);
    std::uniform_real_distribution<float> speedDist(minSpeed, maxSpeed);
    float angle = angleDist(rng);
    float speed = speedDist(rng);
    return { std::cos(angle) * speed, std::sin(angle) * speed };
}

void ParticleSystem::emit(sf::Vector2f position, int count, sf::Color color) {
    std::uniform_real_distribution<float> lifeDist(0.3f, 0.9f);
    std::uniform_real_distribution<float> radiusDist(2.f, 6.f);

    for (int i = 0; i < count; ++i) {
        Particle p;
        p.position    = position;
        p.velocity    = randomVelocity(60.f, 200.f);
        p.color       = color;
        p.maxLifetime = lifeDist(rng);
        p.lifetime    = p.maxLifetime;
        p.radius      = radiusDist(rng);
        particles_.push_back(p);
    }
}

void ParticleSystem::update(float dt) {
    for (auto& p : particles_) {
        p.lifetime -= dt;
        p.position += p.velocity * dt;
        p.velocity *= 0.92f;
    }
    particles_.erase(
        std::remove_if(particles_.begin(), particles_.end(),
            [](const Particle& p){ return p.lifetime <= 0.f; }),
        particles_.end());
}

void ParticleSystem::draw(sf::RenderWindow& window) const {
    for (const auto& p : particles_) {
        float alpha = std::max(0.f, p.lifetime / p.maxLifetime);
        sf::Color c = p.color;
        c.a = static_cast<uint8_t>(255.f * alpha);   // uint8_t instead of sf::Uint8

        drawShape_.setRadius(p.radius);
        drawShape_.setOrigin({ p.radius, p.radius });
        drawShape_.setPosition(p.position);
        drawShape_.setFillColor(c);
        window.draw(drawShape_);
    }
}
