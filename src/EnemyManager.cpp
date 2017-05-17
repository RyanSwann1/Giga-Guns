#include "EnemyManager.h"
#include "RandomNumberGenerator.h"
#include "WorldMap.h"

EnemyManager::EnemyManager()
	: m_enemies(),
	m_elaspedTime(0),
	m_enemySpawnTime(5)
{
}

void EnemyManager::update(float deltaTime, const WorldMap & worldMap)
{
	m_elaspedTime += deltaTime;
	if (m_elaspedTime >= m_enemySpawnTime)
	{
		m_elaspedTime = 0;
		spawnEnemy(worldMap);
	}
}

void EnemyManager::draw(sf::RenderWindow & window)
{
	for (auto& enemy : m_enemies)
	{
		window.draw(enemy);
	}
}

void EnemyManager::spawnEnemy(const WorldMap& worldMap)
{
	const auto& enemySpawnLocationMap = worldMap.getEnemySpawnLayer().getMap();
	const sf::Vector2f spawnLocation = enemySpawnLocationMap.at(RandomNumberGenerator::getRandNumb(0, enemySpawnLocationMap.size() - 1));
	
	sf::RectangleShape rect(sf::Vector2f(32, 32));
	rect.setPosition(spawnLocation);
	rect.setFillColor(sf::Color::Red);
	m_enemies.push_back(rect);
}