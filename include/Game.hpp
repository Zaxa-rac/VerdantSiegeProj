#pragma once
#include <SFML/Graphics.hpp>
#include <vector>
#include <memory>
#include <string>
#include "Player.hpp"
#include "Enemy.hpp"
#include "Projectile.hpp"
#include "Pickup.hpp"
#include "ThornBarrier.hpp"
#include "WaveManager.hpp"
#include "HUD.hpp"
#include "ParticleSystem.hpp"
#include "Constants.hpp"

enum class GameState { MainMenu, Playing, BetweenWaves, Paused, GameOver };

struct DifficultySettings {
    std::string name         { "Normal" };
    float       speedMult    { 1.0f };
    int         enemyCountAdd{ 0 };
    int         playerHP     { VS::PLAYER_MAX_HP };
};

class Game {
public:
    Game();
    void run();

private:
    void handleEvents();
    void update(float dt);
    void render();

    void startNewGame();
    void startWave();
    void endWave();
    bool waveStarted_{ false };
    void showGameOver();
    void togglePause();
    void spawnBonusPickup();

    void updatePlaying(float dt);
    void updateBetweenWaves(float dt);

    void checkCollisions();
    void checkPlayerProjectilesVsEnemies();
    void checkEnemyProjectilesVsPlayer();
    void checkPickupsVsPlayer();
    void checkBarriersVsEnemies(float dt);
    void checkAcidTilesVsEntities(float dt);
    void pruneDeadEntities();

    void renderArena();
    void renderMainMenu();
    void renderPauseMenu();
    void renderGameOver();
    void renderBetweenWaves();

    void handleMenuInput(const sf::Event& event);
    void handlePauseInput(const sf::Event& event);

    int  loadHighScore();
    void saveHighScore(int score);
    bool loadSettings(const std::string& path);

    // ── Window & clock ────────────────────────────────────────────────────
    sf::RenderWindow window_;
    sf::Clock        clock_;
    int brightness_{ 10 }; // 10 is max brightness
    // ── State ─────────────────────────────────────────────────────────────
    GameState          state_       { GameState::MainMenu };
    DifficultySettings difficulty_;
    int                highScore_   { 0 };
    int                menuSelection_{ 0 };
    float              betweenTimer_{ 3.f };

    sf::Color bgColor_;
    float     bgLerpT_{ 0.f };

    // ── Entities ──────────────────────────────────────────────────────────
    std::unique_ptr<Player>                    player_;
    std::vector<std::unique_ptr<Enemy>>        enemies_;
    std::vector<std::unique_ptr<Projectile>>   projectiles_;
    std::vector<std::unique_ptr<Pickup>>       pickups_;
    std::vector<std::unique_ptr<ThornBarrier>> barriers_;
    std::vector<AcidTile>                      acidTiles_;

    // ── Systems ───────────────────────────────────────────────────────────
    WaveManager    waveManager_;
    HUD            hud_;
    ParticleSystem particles_;

    // ── Font ─────────────────────────────────────────────────────────────
    sf::Font font_;
    bool     fontLoaded_{ false };
};
