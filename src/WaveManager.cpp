#include "WaveManager.hpp"
#include "Enemy.hpp"
#include "Constants.hpp"
#include <fstream>
#include <sstream>
#include <cstdlib>
#include <cmath>
#include <random>

static std::mt19937 rng{ std::random_device{}() };

WaveManager::WaveManager() = default;

// ─────────────────────────────────────────────────────────────────────────────
//  loadFromFile  –  parses waves.cfg
//
//  Format (one wave per line):
//    wave <number> beetles <n> mosquitoes <n> sludge <n>
// ─────────────────────────────────────────────────────────────────────────────
bool WaveManager::loadFromFile(const std::string& path) {
    std::ifstream file(path);
    if (!file.is_open()) return false;

    std::string line;
    while (std::getline(file, line)) {
        if (line.empty() || line[0] == '#') continue;

        std::istringstream ss(line);
        std::string token;
        WaveDefinition def;
        int            waveNum = 0;

        while (ss >> token) {
            if (token == "wave")       ss >> waveNum;
            else if (token == "beetles")    ss >> def.beetleCount;
            else if (token == "mosquitoes") ss >> def.mosquitoCount;
            else if (token == "sludge")     ss >> def.sludgeCount;
        }

        if (waveNum > 0) {
            while (static_cast<int>(definitions_.size()) < waveNum)
                definitions_.emplace_back();
            definitions_[waveNum - 1] = def;
        }
    }

    return !definitions_.empty();
}

// ─────────────────────────────────────────────────────────────────────────────
//  isBossWave  –  every VS::BOSS_WAVE_INTERVAL waves
// ─────────────────────────────────────────────────────────────────────────────
bool WaveManager::isBossWave() const {
    return (currentWave_ % VS::BOSS_WAVE_INTERVAL) == 0;
}

// ─────────────────────────────────────────────────────────────────────────────
//  getDefinition  –  falls back to procedural scaling if cfg is missing
// ─────────────────────────────────────────────────────────────────────────────
WaveDefinition WaveManager::getDefinition(int waveNumber) const {
    int idx = waveNumber - 1;
    if (idx >= 0 && idx < static_cast<int>(definitions_.size()))
        return definitions_[idx];

    // Procedural fallback: scale enemies with wave
    WaveDefinition def;
    def.beetleCount   = 3 + waveNumber * 2;
    def.mosquitoCount = waveNumber > 2 ? 1 + waveNumber     : 0;
    def.sludgeCount   = waveNumber > 3 ? (waveNumber - 2)   : 0;
    return def;
}

// ─────────────────────────────────────────────────────────────────────────────
//  randomEdgePosition  –  spawn at one of the four arena edges
// ─────────────────────────────────────────────────────────────────────────────
sf::Vector2f WaveManager::randomEdgePosition(float arenaW, float arenaH) const {
    std::uniform_int_distribution<int>  edgeDist(0, 3);
    std::uniform_real_distribution<float> xDist(0.f, arenaW);
    std::uniform_real_distribution<float> yDist(0.f, arenaH);

    switch (edgeDist(rng)) {
        case 0: return { xDist(rng), -30.f };         // top
        case 1: return { xDist(rng), arenaH + 30.f }; // bottom
        case 2: return { -30.f,       yDist(rng) };   // left
        default:return { arenaW+30.f, yDist(rng) };   // right
    }
}

// ─────────────────────────────────────────────────────────────────────────────
//  spawnWave  –  returns the enemy list for waveNumber
// ─────────────────────────────────────────────────────────────────────────────
std::vector<std::unique_ptr<Enemy>>
WaveManager::spawnWave(int waveNumber, float speedMult,
                        float arenaW,   float arenaH)
{
    std::vector<std::unique_ptr<Enemy>> enemies;

    // Boss wave overrides normal composition
    if ((waveNumber % VS::BOSS_WAVE_INTERVAL) == 0) {
        auto boss = std::make_unique<ScorpionMech>(
            randomEdgePosition(arenaW, arenaH));
        boss->applySpeedMultiplier(speedMult);
        enemies.push_back(std::move(boss));
        // Add a few beetles alongside the boss for pressure
        for (int i = 0; i < waveNumber / VS::BOSS_WAVE_INTERVAL * 2; ++i) {
            auto b = std::make_unique<BeetleBot>(
                randomEdgePosition(arenaW, arenaH));
            b->applySpeedMultiplier(speedMult);
            enemies.push_back(std::move(b));
        }
        return enemies;
    }

    WaveDefinition def = getDefinition(waveNumber);

    for (int i = 0; i < def.beetleCount; ++i) {
        auto e = std::make_unique<BeetleBot>(
            randomEdgePosition(arenaW, arenaH));
        e->applySpeedMultiplier(speedMult);
        enemies.push_back(std::move(e));
    }
    for (int i = 0; i < def.mosquitoCount; ++i) {
        auto e = std::make_unique<MosquitoDrone>(
            randomEdgePosition(arenaW, arenaH));
        e->applySpeedMultiplier(speedMult);
        enemies.push_back(std::move(e));
    }
    for (int i = 0; i < def.sludgeCount; ++i) {
        auto e = std::make_unique<SludgeCrawler>(
            randomEdgePosition(arenaW, arenaH));
        e->applySpeedMultiplier(speedMult);
        enemies.push_back(std::move(e));
    }

    return enemies;
}
