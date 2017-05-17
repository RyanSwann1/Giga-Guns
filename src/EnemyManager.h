#pragma once

#include <SFML\Graphics.hpp>
class WorldMap;
class EnemyManager
{
public:
	EnemyManager();
	void update(float deltaTime, const WorldMap& worldMap);
	void draw(sf::RenderWindow& window);

private:
	std::vector<sf::RectangleShape> m_enemies;
	float m_elaspedTime;
	float m_enemySpawnTime;

	void spawnEnemy(const WorldMap& worldMap);
};