#pragma once

#include "Entity.h"
#include <memory>
#include <SFML/Window/Keyboard.hpp>
#include <SFML/System/Angle.hpp>
#include <SFML/Graphics/Sprite.hpp>

namespace sf { class Sprite; }

class Arrow final : public Entity {
public:
    static constexpr float collisionRadius = 24.0f;

    Arrow() = default;
    virtual ~Arrow() = default;
    
    bool init() override;
    void update(float dt) override;
    void render(sf::RenderTarget& target) const override;

    float arrowMoveSpeed = 800;
    float startingX;
    float arrowMaxDistance = 500;
};
