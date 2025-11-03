#pragma once

#include "IState.h"
#include <memory>

namespace sf { class Text; };

class StateGameOver : public IState
{
public:
    StateGameOver(StateStack& stateStack);
    ~StateGameOver() = default;

    bool init() override;
    void update(float dt) override;
    void render(sf::RenderTarget& target) const override;

public:
    StateStack& m_stateStack;
    const IState* m_pPrevState = nullptr;
    std::unique_ptr<sf::Text> m_pText;
    bool m_hasPauseKeyBeenReleased = false;
};
