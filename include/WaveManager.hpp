#pragma once
#include <vector>
#include <string>
#include <memory>
#include <SFML/System/Vector2.hpp>

class Enemy;

// ─────────────────────────────────────────────────────────────────────────────
//  WaveDefinition
// ─────────────────────────────────────────────────────────────────────────────
struct WaveDefinition {
    int beetleCount   { 0 };
    int mosquitoCount { 0 };
    int sludgeCount   { 0 };
    bool bossWave     { false };
};

// ─────────────────────────────────────────────────────────────────────────────
//  WaveManager
//  – Reads wave composition from config/waves.cfg.
//  – Generates enemy spawn lists on demand.
//  – Tracks whether the current wave is complete.
// ─────────────────────────────────────────────────────────────────────────────
class WaveManager {
public:
    WaveManager();

    // Load config; falls back to procedural generation if file missing
    bool loadFromFile(const std::string& path);

    // Returns a fresh enemy list for the given wave number (1-based)
    std::vector<std::unique_ptr<Enemy>> spawnWave(int waveNumber,
                                                   float speedMult,
                                                   float arenaW,
                                                   float arenaH);

    int  getCurrentWave() const { return currentWave_; }
    void advance()              { ++currentWave_; }
    bool isBossWave()     const;

private:
    int                         currentWave_{ 1 };
    std::vector<WaveDefinition> definitions_;   // loaded from cfg (may be empty)

    WaveDefinition getDefinition(int waveNumber) const;
    sf::Vector2f   randomEdgePosition(float arenaW, float arenaH) const;
};
