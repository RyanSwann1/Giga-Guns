#pragma once

#include "Enemy.h"
#include <memory>
class WorldMap;
class EntityManager;
class EnemyManager
{
public:
	EnemyManager();
	
	const std::vector<std::unique_ptr<Enemy>>& getEnemies() const;
	void removeEnemy(int ID);
	void update(float deltaTime, const WorldMap& worldMap);
	void draw(sf::RenderWindow& window);

private:
	std::vector<std::unique_ptr<Enemy>> m_enemies;
	std::vector<int> m_removals; 
	int m_enemyID;
	float m_elaspedTime;
	float m_enemySpawnTime;

	void spawnEnemy(const WorldMap& worldMap);
	void handleRemovals();
};