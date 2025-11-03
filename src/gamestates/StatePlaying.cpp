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

    // Load font and create text
    const sf::Font* pFont = ResourceManager::getOrLoadFont("Lavigne.ttf");
    if (pFont == nullptr)
        return false;
    // Clock text
    m_pText = std::make_unique<sf::Text>(*pFont);
    if (!m_pText)
        return false;
    m_pText->setStyle(sf::Text::Bold);
    // Restart text
    m_pRestartText = std::make_unique<sf::Text>(*pFont);
    if (!m_pRestartText)
        return false;
    m_pRestartText->setStyle(sf::Text::Bold);
    m_pRestartText->setString("PRESS <R> TO RESTART");
    // Game Over Text
    m_pGameOverText = std::make_unique<sf::Text>(*pFont);
    if (!m_pRestartText)
        return false;
    m_pGameOverText->setStyle(sf::Text::Bold);
    m_pGameOverText->setString("GAME OVER");
    // Hi Score title
    m_pHiScoreText = std::make_unique<sf::Text>(*pFont);
    if (!m_pRestartText)
        return false;
    m_pHiScoreText->setStyle(sf::Text::Bold);
    m_pHiScoreText->setString("HI SCORES:");
    // Score text TODO make into one text type with multiple lines
    m_pScoreText = std::make_unique<sf::Text>(*pFont);
    if (!m_pRestartText) {return false;}
    m_pScoreText->setStyle(sf::Text::Bold);
    // m_pScore1Text = std::make_unique<sf::Text>(*pFont);
    // if (!m_pRestartText) {return false;}
    // m_pScore1Text->setStyle(sf::Text::Bold);
    // m_pScore2Text = std::make_unique<sf::Text>(*pFont);
    // if (!m_pRestartText) {return false;}
    // m_pScore2Text->setStyle(sf::Text::Bold);
    // m_pScore3Text = std::make_unique<sf::Text>(*pFont);
    // if (!m_pRestartText) {return false;}
    // m_pScore3Text->setStyle(sf::Text::Bold);
    // m_pScore4Text = std::make_unique<sf::Text>(*pFont);
    // if (!m_pRestartText) {return false;}
    // m_pScore4Text->setStyle(sf::Text::Bold);
    // m_pScore5Text = std::make_unique<sf::Text>(*pFont);
    // if (!m_pRestartText) {return false;}
    // m_pScore5Text->setStyle(sf::Text::Bold);

    return true;
}

void StatePlaying::update(float dt)
{
    // Game Over and Restart
    // TODO turn this it's own GameOver state!
    if (playerDied) {
        // Save the score in hiScores
        if (!scoreSaved) {
            // Convert time to string
            int totalMs = static_cast<int>(elapsed.asMilliseconds());
            int minutes = (totalMs / 1000) / 60;
            int seconds = (totalMs / 1000) % 60;
            int milliseconds = totalMs % 1000;
            std::ostringstream ss;
            ss << std::setfill('0') << std::setw(2) << minutes << ":"
                << std::setfill('0') << std::setw(2) << seconds << ":"
                << std::setfill('0') << std::setw(2) << milliseconds;
            hiScores.push_back(ss.str());
            // Reorder HI SCORES
            std::sort(hiScores.begin(), hiScores.end(), std::greater<std::string>());
            scoreSaved = true;
        }
        // Restart and clean up
        if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::R)) {
            m_enemies.clear();
            m_arrows.clear();
            scoreClock.restart();
            m_pPlayer->setPosition(sf::Vector2f(200, 752));
            playerDied = false;
            scoreSaved = false;
        }
        return;
    }

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

        if (pEnemy->init())
            m_enemies.push_back(std::move(pEnemy));
    }

    // Shoot arrow with enter key
    m_timeUntilShootArrow -= dt;
    if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::Enter) && m_timeUntilShootArrow < 0.0f) {
        m_timeUntilShootArrow = arrowShootInterval;
        std::unique_ptr<Arrow> pArrow = std::make_unique<Arrow>();
        pArrow->setPosition({m_pPlayer->getPosition().x + 48, m_pPlayer->getPosition().y});
        pArrow->startingX = m_pPlayer->getPosition().x;
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

    // Display clock

    elapsed = scoreClock.getElapsedTime();

    int totalMs = static_cast<int>(elapsed.asMilliseconds());
    int minutes = (totalMs / 1000) / 60;
    int seconds = (totalMs / 1000) % 60;
    int milliseconds = totalMs % 1000;

    std::ostringstream ss;
    ss << std::setfill('0') << std::setw(2) << minutes << ":"
        << std::setfill('0') << std::setw(2) << seconds << ":"
        << std::setfill('0') << std::setw(2) << milliseconds;

    m_pText->setString(ss.str());

    // Detect player and enemy collisions
    
    for (const std::unique_ptr<Enemy>& pEnemy : m_enemies) {
        sf::FloatRect playerBounds = m_pPlayer->m_pSprite->getGlobalBounds();
        sf::FloatRect enemyBounds = pEnemy->m_pSprite->getGlobalBounds();
        if (const std::optional intersection = playerBounds.findIntersection(enemyBounds)) {
            playerDied = true;
            break;
        }
    }

    // Detect arrow and enemy collisions
    for (const std::unique_ptr<Enemy>& pEnemy : m_enemies) {
        for (const std::unique_ptr<Arrow>& pArrow : m_arrows) {
            //sf::FloatRect enemyBounds = pEnemy->m_pSprite->getGlobalBounds();
            sf::FloatRect arrowBounds = pArrow->m_pSprite->getGlobalBounds();
            sf::FloatRect enemyBounds(pEnemy->getPosition(), {48.f, 48.f});
            if (const std::optional intersection = arrowBounds.findIntersection(enemyBounds)) {
                pEnemy->isHit = true;
                pArrow->isHit = true;
            }
        }
    }

    // Clean up entity vectors on death or offscreen

    for (size_t i = 0; i < m_enemies.size();) {
        if (m_enemies[i]->isHit || m_enemies[i]->getPosition().x < -48) {
            m_enemies.erase(m_enemies.begin() + i);
        }
        else {
            ++i;
        }
    }

    for (size_t i = 0; i < m_arrows.size();) {
        if (m_arrows[i]->isHit
        || m_arrows[i]->getPosition().x > m_arrows[i]->startingX + m_arrows[i]->arrowMaxDistance) {
            m_arrows.erase(m_arrows.begin() + i);
        }
        else {
            ++i;
        }
    }
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

    m_pText->setPosition({50.f, 50.f});
    target.draw(*m_pText);

    if (playerDied) {
        m_pRestartText->setPosition({300.f, 300.f});
        target.draw(*m_pRestartText);
        m_pGameOverText->setPosition({300.f, 250.f});
        target.draw(*m_pGameOverText);
    }

    // Draw Hi Scores
    m_pScoreText->setPosition({850, 12});
    m_pScoreText->setString("HI SCORES:");
    target.draw(*m_pScoreText);
    for (size_t i = 0; i < hiScores.size() && i < 5; i++) {
        m_pScoreText->setPosition({850, 12 + static_cast<float>((i + 1) * 36)});
        m_pScoreText->setString(hiScores[i]);
        target.draw(*m_pScoreText);
    }
}
