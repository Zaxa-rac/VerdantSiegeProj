#pragma once
#include <SFML/Graphics.hpp>
#include <cstdint>   // uint8_t

namespace VS {

// ── Window ────────────────────────────────────────────────────────────────────
constexpr unsigned int WINDOW_W     = 1920;
constexpr unsigned int WINDOW_H     = 1080;
constexpr unsigned int TARGET_FPS   = 60;
constexpr char         WINDOW_TITLE[] = "Verdant Siege";

// ── Arena ─────────────────────────────────────────────────────────────────────
constexpr float ARENA_X = 0.f;
constexpr float ARENA_Y = 0.f;
constexpr float ARENA_W = static_cast<float>(WINDOW_W);
constexpr float ARENA_H = static_cast<float>(WINDOW_H);

// ── Player ────────────────────────────────────────────────────────────────────
constexpr int   PLAYER_MAX_HP          = 100;
constexpr float PLAYER_SPEED           = 250.f;
constexpr int   PLAYER_MAX_BARRIERS    = 3;
constexpr float PLAYER_BARRIER_REGEN   = 8.f;
constexpr float PLAYER_SHOOT_COOLDOWN  = 0.25f;
constexpr float PLAYER_RADIUS          = 18.f;

// ── Spore Shot ────────────────────────────────────────────────────────────────
constexpr float SPORE_SPEED   = 700.f;
constexpr int   SPORE_DAMAGE  = 20;
constexpr float SPORE_RANGE   = 700.f;
constexpr float SPORE_RADIUS  = 6.f;

// ── Thorn Barrier ─────────────────────────────────────────────────────────────
constexpr float BARRIER_LIFETIME    = 8.f;
constexpr int   BARRIER_DPS         = 15;
constexpr float BARRIER_W           = 80.f;
constexpr float BARRIER_H           = 20.f;

// ── Enemy – BeetleBot ─────────────────────────────────────────────────────────
constexpr float BEETLE_SPEED   = 90.f;
constexpr int   BEETLE_HP      = 120;
constexpr int   BEETLE_DAMAGE  = 15;
constexpr float BEETLE_RADIUS  = 20.f;

// ── Enemy – MosquitoDrone ─────────────────────────────────────────────────────
constexpr float MOSQUITO_SPEED        = 175.f;
constexpr int   MOSQUITO_HP           = 40;
constexpr int   MOSQUITO_DAMAGE       = 20;
constexpr float MOSQUITO_ORBIT_RADIUS = 250.f;
constexpr float MOSQUITO_FIRE_RATE    = 2.f;
constexpr float MOSQUITO_RADIUS       = 14.f;

// ── Enemy – SludgeCrawler ─────────────────────────────────────────────────────
constexpr float SLUDGE_SPEED         = 60.f;
constexpr int   SLUDGE_HP            = 200.f;
constexpr int   SLUDGE_DAMAGE        = 5;
constexpr float SLUDGE_SLOW_FACTOR   = 0.5f;
constexpr float SLUDGE_TRAIL_TTL     = 9.f;
constexpr float SLUDGE_RADIUS        = 16.f;

// ── Enemy – ScorpionMech (boss) ───────────────────────────────────────────────
constexpr float BOSS_SPEED         = 120.f;
constexpr int   BOSS_HP            = 800;
constexpr int   BOSS_DAMAGE        = 50;
constexpr float BOSS_PHASE2_THRESH = 0.5f;
constexpr float BOSS_RADIUS        = 40.f;
constexpr int   BOSS_WAVE_INTERVAL = 5;

// ── Pickups ───────────────────────────────────────────────────────────────────
constexpr int   POWER_CELL_VALUE     = 10;
constexpr int   REPAIR_KIT_HP        = 30;
constexpr float FERTILISER_DURATION  = 10.f;
constexpr float FERTILISER_FIRE_MULT = 2.f;
constexpr float PICKUP_RADIUS        = 14.f;

// ── Scoring ───────────────────────────────────────────────────────────────────
constexpr int WAVE_BONUS_MULTIPLIER = 100;

// ── Files ─────────────────────────────────────────────────────────────────────
constexpr char HIGHSCORE_FILE[]  = "highscore.dat";
constexpr char WAVES_CFG[]       = "config/waves.cfg";
constexpr char SETTINGS_CFG[]    = "config/settings.cfg";

// ── Colours ───────────────────────────────────────────────────────────────────────────
inline const sf::Color COLOR_HP_BAR       {  80, 200,  80 };
inline const sf::Color COLOR_HP_BG        {  60,  60,  60 };
inline const sf::Color COLOR_BARRIER_TINT {  50, 200, 100, 180 };
inline const sf::Color COLOR_ACID_TRAIL   {  90, 180,  30, 120 };

} 
