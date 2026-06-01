#pragma once
#include <SFML/Graphics.hpp>
#include <unordered_map>
#include <string>

struct AnimationData {
    int   frameWidth;
    int   frameHeight;
    int   frameCount;
    int   startY;       // The Y-pixel coordinate where this animation starts on the sheet
    float frameDuration; // How long each frame lasts (e.g., 0.1f seconds)
};

class Animator {
public:
    explicit Animator(sf::Sprite& sprite);

    void addAnimation(const std::string& name, int width, int height, int count, int startY, float duration);
    void play(const std::string& name);
    void update(float dt);

    std::string getCurrentAnimation() const { return currentAnimName_; }

private:
    sf::Sprite& sprite_;
    std::unordered_map<std::string, AnimationData> animations_;
    
    std::string currentAnimName_;
    int         currentFrame_{ 0 };
    float       elapsedTime_{ 0.f };
};