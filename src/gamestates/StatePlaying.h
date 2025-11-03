#pragma once

#include "IState.h"
#include "entities/Player.h"
#include "entities/Enemy.h"
#include "entities/Arrow.h"
#include <SFML/Graphics/Texture.hpp>
#include <SFML/Graphics/VertexArray.hpp>
#include <SFML/Graphics/Text.hpp>
#include <SFML/Graphics/RectangleShape.hpp>
#include <SFML/System/Clock.hpp>
#include <SFML/System/Time.hpp>
#include <sstream>


class StatePlaying : public IState
{
public:
    StatePlaying(StateStack& stateStack);
    ~StatePlaying() = default;

    bool init() override;
    void update(float dt) override;
    void render(sf::RenderTarget& target) const override;

    std::unique_ptr<sf::Text> m_pText;
    std::unique_ptr<sf::Text> m_pRestartText;
    std::unique_ptr<sf::Text> m_pGameOverText;
    std::unique_ptr<sf::Text> m_pScoreText;

    sf::Clock scoreClock;
	sf::Time elapsed;

    std::vector<std::string> hiScores;

    std::string lastScore;

    bool playerDied = false;
    bool scoreSaved = false;
    

private:
    // Set enemy spawn rate
    float startingEnemySpawningInterval = 1.5f;
    float enemySpawnInterval = startingEnemySpawningInterval;
    float m_timeUntilEnemySpawn = enemySpawnInterval;

    // Enemy speed increase timer
    static constexpr const float speedIncreaseInterval = 1.0f;
    float speedIncreaseTimer = speedIncreaseInterval;

    // Set minimum time between arrow shots
    static constexpr const float arrowShootInterval = 0.2f;
    float m_timeUntilShootArrow = 0.0f;

    StateStack& m_stateStack;
    std::unique_ptr<Player> m_pPlayer;
    std::vector<std::unique_ptr<Enemy>> m_enemies;
    std::vector<std::unique_ptr<Arrow>> m_arrows;
    sf::RectangleShape m_ground;
    bool m_hasPauseKeyBeenReleased = true;

    float startingEnemyMoveSpeed = 300;
    float enemyMoveSpeed = startingEnemyMoveSpeed;

    void updateCollisions();
};
