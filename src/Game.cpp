#include "Game.hpp"
#include "Constants.hpp"
#include "AssetManager.hpp"
#include <fstream>
#include <sstream>
#include <algorithm>
#include <random>
#include <cmath>
#include <cstdint>


static std::mt19937 rng{ std::random_device{}() };

// ─────────────────────────────────────────────────────────────────────────────
//  Constructor
// ─────────────────────────────────────────────────────────────────────────────
Game::Game()
    : window_(sf::VideoMode({ VS::WINDOW_W, VS::WINDOW_H }), VS::WINDOW_TITLE)
{
    window_.setFramerateLimit(VS::TARGET_FPS);

    if (font_.openFromFile("assets/fonts/font.ttf")) {
        fontLoaded_ = true;
        hud_.loadFont("assets/fonts/font.ttf");
    }

    AssetManager::getInstance().loadTexture("player", "assets/textures/player-1.png");
    AssetManager::getInstance().loadTexture("beetle", "assets/textures/beetle-1.png");
    AssetManager::getInstance().loadTexture("mosquito", "assets/textures/mosquito-1.png");
    AssetManager::getInstance().loadTexture("sludge", "assets/textures/sludge-1.png");
    AssetManager::getInstance().loadTexture("scorpion", "assets/textures/scorpion-1.png");

    loadSettings(VS::SETTINGS_CFG);
    highScore_ = loadHighScore();
    bgColor_   = sf::Color{ 10, 30, 40 };
}


void Game::run() {
    while (window_.isOpen()) {
        float dt = clock_.restart().asSeconds();
        dt = std::min(dt, 0.05f);
        handleEvents();
        update(dt);
        render();
    }
}


void Game::handleEvents() {
    while (const std::optional event = window_.pollEvent()) {

        if (event->is<sf::Event::Closed>()) {
            window_.close();
            return;
        }

        if (const auto* kp = event->getIf<sf::Event::KeyPressed>()) {

            if (state_ == GameState::MainMenu) {
                if (kp->code == sf::Keyboard::Key::Enter && menuSelection_ == 0)
                    startNewGame();
                if (kp->code == sf::Keyboard::Key::Up)
                    menuSelection_ = (menuSelection_ + 3) % 4;
                if (kp->code == sf::Keyboard::Key::Down)
                    menuSelection_ = (menuSelection_ + 1) % 4;

                // Difficulty Toggle
                if (menuSelection_ == 3 && (kp->code == sf::Keyboard::Key::Left || kp->code == sf::Keyboard::Key::Right)) {
                    if (difficulty_.name == "Normal") {
                        difficulty_ = { "Hard", 1.5f, 5, 80 };   // Faster, more enemies, less Player HP
                    } else if (difficulty_.name == "Hard") {
                        difficulty_ = { "Easy", 0.75f, -2, 150 };// Slower, fewer enemies, more Player HP
                    } else {
                        difficulty_ = { "Normal", 1.0f, 0, 100 };// Default
                    }
                }
                //Brightness Adjustment                
                if (menuSelection_ == 1 && (kp->code == sf::Keyboard::Key::Left)) {
                    if (brightness_ > 2) brightness_ -= 1; // Lower brightness
                }
                if (menuSelection_ == 1 && (kp->code == sf::Keyboard::Key::Right)) {
                    if (brightness_ < 10) brightness_ += 1; // Raise brightness
                }

            } else if (state_ == GameState::Playing) {
                if (kp->code == sf::Keyboard::Key::Escape) togglePause();

            } else if (state_ == GameState::Paused) {
                if (kp->code == sf::Keyboard::Key::Escape) togglePause();
                if (kp->code == sf::Keyboard::Key::R)      startNewGame();
                if (kp->code == sf::Keyboard::Key::Q)      state_ = GameState::MainMenu;

            } else if (state_ == GameState::GameOver) {
                if (kp->code == sf::Keyboard::Key::Enter)  state_ = GameState::MainMenu;

            } else if (state_ == GameState::BetweenWaves) {
                if (kp->code == sf::Keyboard::Key::Enter)  startWave();
            }
        }

        if (const auto* mb = event->getIf<sf::Event::MouseButtonPressed>()) {
            if (state_ == GameState::Playing && player_) {
                if (mb->button == sf::Mouse::Button::Left)  player_->shootRequested_   = true;
                if (mb->button == sf::Mouse::Button::Right) player_->barrierRequested_ = true;
            }
        }
    }
}

// ─────────────────────────────────────────────────────────────────────────────
//  update
// ─────────────────────────────────────────────────────────────────────────────
void Game::update(float dt) {
    switch (state_) {
        case GameState::Playing:      updatePlaying(dt);      break;
        case GameState::BetweenWaves: updateBetweenWaves(dt); break;
        default: break;
    }
}

void Game::updatePlaying(float dt) {
    if (!player_) return;

    player_->handleInput(window_, dt);
    player_->update(dt);

    if (player_->shootRequested_) {
        player_->shootRequested_ = false;
        auto proj = player_->shoot(player_->mouseWorldPos_);
        if (proj) projectiles_.push_back(std::move(proj));
    }
    if (player_->barrierRequested_) {
        player_->barrierRequested_ = false;
        auto bar = player_->placeBarrier(player_->mouseWorldPos_);
        if (bar) {
            if (barriers_.size() >= static_cast<size_t>(VS::PLAYER_MAX_BARRIERS))
                barriers_.erase(barriers_.begin());
            barriers_.push_back(std::move(bar));
        }
    }

    for (auto& e : enemies_) {
        if (!e->isAlive()) continue;
        e->move(*player_, dt);
        e->attack(*player_, dt);

        if (auto* sc = dynamic_cast<SludgeCrawler*>(e.get()))
            acidTiles_.push_back(sc->makeTrailTile());
        if (auto* md = dynamic_cast<MosquitoDrone*>(e.get())) {
            auto blob = md->tryShoot(*player_);
            if (blob) projectiles_.push_back(std::move(blob));
        }
        if (auto* sm = dynamic_cast<ScorpionMech*>(e.get())) {
            auto blob = sm->tryShoot(*player_);
            if (blob) projectiles_.push_back(std::move(blob));
        }
    }

    for (auto& p : projectiles_) p->update(dt);
    for (auto& b : barriers_)    b->update(dt);
    for (auto& t : acidTiles_)   { t.ttl -= dt; if (t.ttl <= 0.f) t.active = false; }

    particles_.update(dt);
    checkCollisions();

    bool anyAlive = std::any_of(enemies_.begin(), enemies_.end(),
        [](const auto& e){ return e->isAlive(); });
    if (waveStarted_ && !anyAlive) { endWave(); return; }

    if (player_->isDead())   { showGameOver(); return; }

    int alive = static_cast<int>(std::count_if(enemies_.begin(), enemies_.end(),
        [](const auto& e){ return e->isAlive(); }));
    hud_.update(*player_, waveManager_.getCurrentWave(), alive);

    // Background lerp night -> dawn
    bgLerpT_ = std::min(1.f, bgLerpT_ + dt * 0.01f);
    sf::Color dawn{ 120, 80, 30 }, night{ 10, 30, 40 };
    bgColor_.r = static_cast<uint8_t>(night.r + (dawn.r - night.r) * bgLerpT_);
    bgColor_.g = static_cast<uint8_t>(night.g + (dawn.g - night.g) * bgLerpT_);
    bgColor_.b = static_cast<uint8_t>(night.b + (dawn.b - night.b) * bgLerpT_);
    
    pruneDeadEntities();

    window_.setTitle("Enemies alive: " + std::to_string(
     std::count_if(enemies_.begin(), enemies_.end(),
         [](const auto& e){ return e->isAlive(); }
        )));   
}

void Game::updateBetweenWaves(float dt) {
    betweenTimer_ -= dt;
    if (betweenTimer_ <= 0.f) startWave();
    particles_.update(dt);
}

// ─────────────────────────────────────────────────────────────────────────────
//  Collisions
// ─────────────────────────────────────────────────────────────────────────────
void Game::checkCollisions() {
    checkPlayerProjectilesVsEnemies();
    checkEnemyProjectilesVsPlayer();
    checkPickupsVsPlayer();
    checkBarriersVsEnemies(0.016f);
    checkAcidTilesVsEntities(0.016f);
}

void Game::checkPlayerProjectilesVsEnemies() {
    for (auto& proj : projectiles_) {
        if (!proj->isAlive() || proj->getOwner() != Projectile::Owner::Player) continue;
        for (auto& enemy : enemies_) {
            if (!enemy->isAlive()) continue;
            if (proj->getBounds().findIntersection(enemy->getBounds())) {
                enemy->takeDamage(proj->getDamage());
                proj->onHit(*enemy);
                particles_.emit(proj->getPosition());
                if (!enemy->isAlive()) {
                    auto pickup = enemy->onDeath();
                    if (pickup) pickups_.push_back(std::move(pickup));
                    player_->addScore(VS::POWER_CELL_VALUE);
                }
                break;
            }
        }
    }
}

void Game::checkEnemyProjectilesVsPlayer() {
    for (auto& proj : projectiles_) {
        if (!proj->isAlive() || proj->getOwner() != Projectile::Owner::Enemy) continue;
        if (proj->getBounds().findIntersection(player_->getBounds())) {
            player_->takeDamage(proj->getDamage());
            proj->onHit(*player_);
        }
    }
}

void Game::checkPickupsVsPlayer() {
    for (auto& pickup : pickups_) {
        if (!pickup->isAlive() || pickup->isCollected()) continue;
        if (pickup->getBounds().findIntersection(player_->getBounds()))
            pickup->onCollect(*player_);
    }
}

void Game::checkBarriersVsEnemies(float dt) {
    for (auto& barrier : barriers_) {
        if (!barrier->isAlive()) continue;
        
        for (auto& enemy : enemies_) {
            if (!enemy->isAlive()) continue;
            
            float dist = std::hypot(barrier->getPosition().x - enemy->getPosition().x,
                                    barrier->getPosition().y - enemy->getPosition().y);
            if (dist < 105.f) {

                enemy->takeDamage(static_cast<int>(barrier->getDamage() * dt) + 1); 
               
                sf::Vector2f pushDir = enemy->getPosition() - barrier->getPosition();
                float pushDist = std::hypot(pushDir.x, pushDir.y);
                
                if (pushDist > 0.001f) {
                    pushDir.x /= pushDist;
                    pushDir.y /= pushDist;
                    
                    enemy->pushBack(pushDir * 1000.f * dt); 
                }
                if (!enemy->isAlive()) {
                    auto pickup = enemy->onDeath();
                    if (pickup) pickups_.push_back(std::move(pickup));
                }
            }
        }
    }
}   

void Game::checkAcidTilesVsEntities(float /*dt*/) {
    constexpr float tileRadius = 16.f;
    for (auto& tile : acidTiles_) {
        if (!tile.active) continue;
        float dist = std::hypot(tile.position.x - player_->getPosition().x,
                                 tile.position.y - player_->getPosition().y);
        if (dist < tileRadius)
            player_->applySpeedMultiplier(VS::SLUDGE_SLOW_FACTOR, 0.2f);
    }
}

void Game::pruneDeadEntities() {
    auto dead = [](const auto& e){ return !e->isAlive(); };
    enemies_.erase(    std::remove_if(enemies_.begin(),     enemies_.end(),     dead), enemies_.end());
    projectiles_.erase(std::remove_if(projectiles_.begin(), projectiles_.end(), dead), projectiles_.end());
    pickups_.erase(    std::remove_if(pickups_.begin(),     pickups_.end(),     dead), pickups_.end());
    barriers_.erase(   std::remove_if(barriers_.begin(),    barriers_.end(),    dead), barriers_.end());
    acidTiles_.erase(  std::remove_if(acidTiles_.begin(),   acidTiles_.end(),
        [](const AcidTile& t){ return !t.active; }), acidTiles_.end());
}

// ─────────────────────────────────────────────────────────────────────────────
//  State transitions
// ─────────────────────────────────────────────────────────────────────────────
void Game::startNewGame() {
    player_ = std::make_unique<Player>();
    enemies_.clear(); projectiles_.clear();
    pickups_.clear(); barriers_.clear();
    acidTiles_.clear(); particles_.clear();
    bgLerpT_ = 0.f;
    bgColor_ = sf::Color{ 10, 30, 40 };
    waveManager_ = WaveManager();
    waveManager_.loadFromFile(VS::WAVES_CFG);
    state_ = GameState::Playing;
    startWave();
}

void Game::startWave() {
    state_       = GameState::Playing;
    waveStarted_ = true; 
    enemies_     = waveManager_.spawnWave(
        waveManager_.getCurrentWave(), difficulty_.speedMult,
        VS::ARENA_W, VS::ARENA_H);
}

void Game::endWave() {
    waveStarted_ = false;
    if (player_)
        player_->addScore(VS::WAVE_BONUS_MULTIPLIER * waveManager_.getCurrentWave());
    waveManager_.advance();
    spawnBonusPickup();
    betweenTimer_ = 3.f;
    state_        = GameState::BetweenWaves;
}

void Game::spawnBonusPickup() {
    std::uniform_real_distribution<float> xd(100.f, VS::ARENA_W - 100.f);
    std::uniform_real_distribution<float> yd(100.f, VS::ARENA_H - 100.f);
    pickups_.push_back(
        std::make_unique<FertiliserCanister>(sf::Vector2f{ xd(rng), yd(rng) }));
}

void Game::showGameOver() {
    if (player_ && player_->getScore() > highScore_) {
        highScore_ = player_->getScore();
        saveHighScore(highScore_);
    }
    state_ = GameState::GameOver;
}

void Game::togglePause() {
    state_ = (state_ == GameState::Paused) ? GameState::Playing : GameState::Paused;
}

// ─────────────────────────────────────────────────────────────────────────────
//  render
// ─────────────────────────────────────────────────────────────────────────────
void Game::render() {
    window_.clear(bgColor_);
    switch (state_) {
        case GameState::MainMenu:     renderMainMenu();                       break;
        case GameState::Playing:      renderArena();                          break;
        case GameState::BetweenWaves: renderArena(); renderBetweenWaves();    break;
        case GameState::Paused:       renderArena(); renderPauseMenu();       break;
        case GameState::GameOver:     renderArena(); renderGameOver();        break;
    }
    if (brightness_ < 10) {
        sf::RectangleShape darkOverlay({ VS::WINDOW_W, VS::WINDOW_H });
        
        // Use standard C++ std::uint8_t instead of sf::Uint8
        std::uint8_t alpha = static_cast<std::uint8_t>(255 - (brightness_ * 25));
        darkOverlay.setFillColor(sf::Color{ 0, 0, 0, alpha });
    
        window_.draw(darkOverlay);
    }
    window_.display();
}

void Game::renderArena() {
    sf::CircleShape acidShape(16.f);
    acidShape.setOrigin({ 16.f, 16.f });
    acidShape.setFillColor(VS::COLOR_ACID_TRAIL);
    for (const auto& t : acidTiles_) {
        if (t.active) { acidShape.setPosition(t.position); window_.draw(acidShape); }
    }
    for (auto& e  : enemies_)     e->draw(window_);
    for (auto& p  : projectiles_) p->draw(window_);
    for (auto& pk : pickups_)     pk->draw(window_);
    for (auto& b  : barriers_)    b->draw(window_);
    if (player_) player_->draw(window_);
    particles_.draw(window_);
    hud_.draw(window_);
}

// ── Menu render helpers — all sf::Text objects created locally ────────────────


void Game::renderMainMenu() {
    if (!fontLoaded_) return;

    sf::Text title(font_, "Verdant Siege", 64);
    title.setFillColor(sf::Color{ 80, 220, 80 });
    title.setPosition({ VS::WINDOW_W / 2.f - title.getGlobalBounds().size.x / 2.f, 120.f });
    window_.draw(title);

    std::string diffLabel = "Difficulty: " + difficulty_.name;
    std::string scoreLabel = "High Score: " + std::to_string(highScore_);
    std::string labels[] = { "Start Game", "Settings (Coming Soon)", scoreLabel, diffLabel };
    
    for (int i = 0; i < 4; ++i) {
        sf::Text item(font_, labels[i], 28);
        item.setFillColor(i == menuSelection_
            ? sf::Color{ 80, 220, 80 }
            : sf::Color{ 180, 180, 180 });
        item.setPosition({ VS::WINDOW_W / 2.f - 120.f, 280.f + i * 56.f });
        window_.draw(item);
    }
}

void Game::renderPauseMenu() {
    if (!fontLoaded_) return;
    sf::Text t(font_, "PAUSED\n\nESC - Resume\nR - Restart\nQ - Main Menu", 36);
    t.setFillColor(sf::Color::White);
    t.setPosition({ VS::WINDOW_W / 2.f - 140.f, VS::WINDOW_H / 2.f - 80.f });
    window_.draw(t);
}

void Game::renderGameOver() {
    if (!fontLoaded_) return;
    std::string msg = "GAME OVER\n\nScore: " +
        std::to_string(player_ ? player_->getScore() : 0) +
        "\nHigh Score: " + std::to_string(highScore_) +
        "\n\nEnter - Menu";
    sf::Text t(font_, msg, 40);
    t.setFillColor(sf::Color{ 220, 60, 60 });
    t.setPosition({ VS::WINDOW_W / 2.f - 160.f, VS::WINDOW_H / 2.f - 100.f });
    window_.draw(t);
}

void Game::renderBetweenWaves() {
    if (!fontLoaded_) return;
    std::string msg = "Wave " +
        std::to_string(waveManager_.getCurrentWave() - 1) +
        " complete!\n\nNext wave in " +
        std::to_string(static_cast<int>(betweenTimer_)) + "s\n(Enter to skip)";
    sf::Text t(font_, msg, 32);
    t.setFillColor(sf::Color{ 80, 220, 80 });
    t.setPosition({ VS::WINDOW_W / 2.f - 160.f, VS::WINDOW_H / 2.f - 60.f });
    window_.draw(t);
}

// ─────────────────────────────────────────────────────────────────────────────
//  Persistence
// ─────────────────────────────────────────────────────────────────────────────
int Game::loadHighScore() {
    std::ifstream f(VS::HIGHSCORE_FILE);
    int score = 0;
    if (f) f >> score;
    return score;
}
void Game::saveHighScore(int score) {
    std::ofstream f(VS::HIGHSCORE_FILE);
    if (f) f << score;
}
bool Game::loadSettings(const std::string& path) {
    std::ifstream file(path);
    if (!file.is_open()) return false;
    std::string key;
    while (file >> key) {
        if (key == "speed_mult")      file >> difficulty_.speedMult;
        if (key == "enemy_count_add") file >> difficulty_.enemyCountAdd;
        if (key == "player_hp")       file >> difficulty_.playerHP;
        if (key == "difficulty_name") file >> difficulty_.name;
    }
    return true;
}
