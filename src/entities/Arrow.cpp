#include "Arrow.h"
#include "ResourceManager.h"
#include <cmath>
#include <SFML/Graphics/RenderTarget.hpp>
#include <SFML/Graphics/Sprite.hpp>

bool Arrow::init() {
    const sf::Texture* pTexture = ResourceManager::getOrLoadTexture("arrow.png");
    if (pTexture == nullptr)
        return false;

    m_pSprite = std::make_unique<sf::Sprite>(*pTexture);
    if (!m_pSprite)
        return false;

    m_pSprite->setPosition(m_position);

    return true;
}

void Arrow::update(float dt) {
    m_position.x += arrowMoveSpeed * dt;
}

void Arrow::render(sf::RenderTarget& target) const {
    m_pSprite->setPosition(m_position);
    target.draw(*m_pSprite);
}
