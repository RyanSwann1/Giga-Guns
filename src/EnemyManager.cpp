#include "EnemyManager.h"
#include "RandomNumberGenerator.h"
#include "WorldMap.h"
#include <algorithm>
#include <assert.h>

EnemyManager::EnemyManager()
	: m_enemies(),
	m_elaspedTime(0),
	m_enemySpawnTime(5)
{
}

const std::vector<std::unique_ptr<Enemy>>& EnemyManager::getEnemies() const
{
	return m_enemies;
}

void EnemyManager::removeEnemy(int ID)
{
	if (std::find(m_removals.cbegin(), m_removals.cend(), ID) == m_removals.cend())
	{
		m_removals.push_back(ID);
	}
}

void EnemyManager::update(float deltaTime, const WorldMap & worldMap)
{
	m_elaspedTime += deltaTime;
	if (m_elaspedTime >= m_enemySpawnTime)
	{
		m_elaspedTime = 0;
		spawnEnemy(worldMap);
	}

	handleRemovals();
}

void EnemyManager::draw(sf::RenderWindow & window)
{
	for (auto& enemy : m_enemies)
	{
		enemy->draw(window);
	}
}

void EnemyManager::spawnEnemy(const WorldMap& worldMap)
{
	const auto& enemySpawnLocationMap = worldMap.getEnemySpawnLayer().getMap();
	const sf::Vector2f spawnLocation = enemySpawnLocationMap.at(RandomNumberGenerator::getRandNumb(0, enemySpawnLocationMap.size() - 1));
	
	m_enemies.emplace_back(std::make_unique<Enemy>(m_enemyID, spawnLocation));
	++m_enemyID;
}

void EnemyManager::handleRemovals()
{
	for (const auto& enemyID : m_removals)
	{
		auto iter = std::find_if(m_enemies.begin(), m_enemies.end(), [enemyID](const auto& enemy) {return enemy->getID() == enemyID; });
		assert(iter != m_enemies.cend());
		m_enemies.erase(iter);
	}

	m_removals.clear();
}