#pragma once
#include <SFML/Graphics.hpp>
#include <vector>

// ─────────────────────────────────────────────────────────────────────────────
//  Particle  –  a single leaf / spore particle
// ─────────────────────────────────────────────────────────────────────────────
struct Particle {
    sf::Vector2f position;
    sf::Vector2f velocity;
    sf::Color    color;
    float        lifetime;    // seconds remaining
    float        maxLifetime;
    float        radius;
};

// ─────────────────────────────────────────────────────────────────────────────
//  ParticleSystem  –  emits leaf/spore bursts on SporeShot impact.
//  Particles are drawn as coloured circles that fade out over their lifetime.
// ─────────────────────────────────────────────────────────────────────────────
class ParticleSystem {
public:
    ParticleSystem() = default;

    // Emit a burst of particles at the given world position
    void emit(sf::Vector2f position, int count = 12,
              sf::Color color = sf::Color{ 80, 220, 80 });

    void update(float dt);
    void draw(sf::RenderWindow& window) const;

    void clear() { particles_.clear(); }

private:
    std::vector<Particle>   particles_;
    mutable sf::CircleShape drawShape_{ 4.f };

    static sf::Vector2f randomVelocity(float minSpeed, float maxSpeed);
};
