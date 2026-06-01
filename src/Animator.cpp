#include "Animator.hpp"

Animator::Animator(sf::Sprite& sprite) : sprite_(sprite) {}

void Animator::addAnimation(const std::string& name, int width, int height, int count, int startY, float duration) {
    animations_[name] = { width, height, count, startY, duration };
}

void Animator::play(const std::string& name) {
    if (currentAnimName_ == name) return; // Already playing

    currentAnimName_ = name;
    currentFrame_    = 0;
    elapsedTime_     = 0.f;

    // Set the very first frame immediately
    const auto& anim = animations_[name];
    sprite_.setTextureRect(sf::IntRect({ 0, anim.startY }, { anim.frameWidth, anim.frameHeight }));
}

void Animator::update(float dt) {
    if (currentAnimName_.empty()) return;

    elapsedTime_ += dt;
    const auto& anim = animations_[currentAnimName_];

    if (elapsedTime_ >= anim.frameDuration) {
        elapsedTime_ -= anim.frameDuration;
        currentFrame_ = (currentFrame_ + 1) % anim.frameCount; // Loop back to 0

        int currentX = currentFrame_ * anim.frameWidth;
        sprite_.setTextureRect(sf::IntRect({ currentX, anim.startY }, { anim.frameWidth, anim.frameHeight }));
    }
}