#include "StatePlaying.h"
#include "StatePaused.h"
#include "StateStack.h"
#include "ResourceManager.h"
#include <memory>
#include <iostream>
#include <SFML/Graphics/RenderTarget.hpp>

StatePlaying::StatePlaying(StateStack& stateStack)
    : m_stateStack(stateStack)
{
}

bool StatePlaying::init()
{
    m_ground.setSize({1024.0f, 256.0f});
    m_ground.setPosition({0.0f, 800.0f});
    sf::Color grey(128, 128, 128);
    m_ground.setFillColor(grey);

    m_pPlayer = std::make_unique<Player>();
    if (!m_pPlayer || !m_pPlayer->init())
        return false;

    m_pPlayer->setPosition(sf::Vector2f(200, 752));

    return true;
}

void StatePlaying::update(float dt)
{
    // Enemy spawner
    m_timeUntilEnemySpawn -= dt;
    if (m_timeUntilEnemySpawn < 0.0f) {
        m_timeUntilEnemySpawn = enemySpawnInterval;
        std::unique_ptr<Enemy> pEnemy = std::make_unique<Enemy>();
        // Set spawn position of enemy
        float randomInt = std::rand() % 4;
        float startingY = 752;
        float variationY = randomInt * 48;
        pEnemy->setPosition(sf::Vector2f(1000, startingY - variationY));
        //pEnemy->setPosition(sf::Vector2f(1000, startingY));

        if (pEnemy->init())
            m_enemies.push_back(std::move(pEnemy));
    }

    // Shoot arrow with enter key
    m_timeUntilShootArrow -= dt;
    if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::Enter) && m_timeUntilShootArrow < 0.0f) {
        m_timeUntilShootArrow = arrowShootInterval;
        std::cout << "pressed enter\n";
        std::unique_ptr<Arrow> pArrow = std::make_unique<Arrow>();
        pArrow->setPosition({m_pPlayer->getPosition().x + 48, m_pPlayer->getPosition().y});
        if (pArrow->init())
            m_arrows.push_back(std::move(pArrow));
    }

    // Pause with escape key
    bool isPauseKeyPressed = sf::Keyboard::isKeyPressed(sf::Keyboard::Key::Escape);
    m_hasPauseKeyBeenReleased |= !isPauseKeyPressed;
    if (m_hasPauseKeyBeenReleased && isPauseKeyPressed) {
        m_hasPauseKeyBeenReleased = false;
        m_stateStack.push<StatePaused>();
    }

    // Update player and enemy
    m_pPlayer->update(dt);
    for (const std::unique_ptr<Enemy>& pEnemy : m_enemies){
        pEnemy->update(dt);
    }
    for (const std::unique_ptr<Arrow>& pArrow : m_arrows){
        pArrow->update(dt);
    }

    // Detect collisions
    bool playerDied = false;
    for (const std::unique_ptr<Enemy>& pEnemy : m_enemies)
    {
        sf::FloatRect playerBounds = m_pPlayer->m_pSprite->getGlobalBounds();
        sf::FloatRect enemyBounds = pEnemy->m_pSprite->getGlobalBounds();
        if (const std::optional intersection = playerBounds.findIntersection(enemyBounds)) {
            playerDied = true;
            break;
        }
    }

    // End Playing State on player death
    if (playerDied)
        m_stateStack.popDeferred();
}

void StatePlaying::render(sf::RenderTarget& target) const
{
    // Draw order
    for (const std::unique_ptr<Enemy>& pEnemy : m_enemies)
        pEnemy->render(target);
    for (const std::unique_ptr<Arrow>& pArrow : m_arrows)
        pArrow->render(target);
    m_pPlayer->render(target);
    target.draw(m_ground);
}
