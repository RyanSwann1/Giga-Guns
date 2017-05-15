#pragma once

#include <SFML\Graphics.hpp>
class WorldMap;
class EnemyManager
{
	class Enemy
	{
	public:
		Enemy(const sf::Vector2f& size, const sf::Vector2f& position);

		void draw(sf::RenderWindow& window);

	private:
		sf::RectangleShape m_rect;
	};

public:
	EnemyManager();
	void update(float deltaTime, const WorldMap& worldMap);
	void draw(sf::RenderWindow& window);

private:
	std::vector<Enemy> m_enemies;
	float m_elaspedTime;
	float m_enemySpawnTime;

	void spawnEnemy(const WorldMap& worldMap);
};