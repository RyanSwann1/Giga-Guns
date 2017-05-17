#pragma once

#include "Direction.h"
#include "Gun.h"
#include <vector>
#include <memory>
class WorldMap;
class EnemyManager;
enum class Direction;
class GunManager
{
	class Bullet
	{
	public:
		Bullet(GunManager& gunManager, const sf::Vector2f& spawnPosition, Direction moveDirection, int ID);
		int getID() const;
		const sf::Vector2f& getPosition() const;
		void update(EnemyManager& enemyManager, const WorldMap& worldMap, float deltaTime);
		void draw(sf::RenderWindow& window);

	private:
		const float m_movementSpeed;
		const Direction m_movementDirection;
		const int m_ID;
		GunManager& m_gunManager;
		sf::RectangleShape m_rect;

		void handleCollisions(EnemyManager& enemyManager, const WorldMap& worldMap);
	};

public:
	GunManager();
	void fireEquippedGun(const sf::Vector2f& spawnPosition, Direction moveDirection);
	void update(EnemyManager& enemyManager, const WorldMap& worldMap, float deltaTime);
	void draw(sf::RenderWindow& window);
	void removeBullet(int ID);

private:
	std::vector<Gun> m_guns;
	std::vector<std::unique_ptr<Bullet>> m_bullets;
	std::vector<int> m_bulletRemovals;
	Gun* m_equippedGun;
	int m_bulletID;

	void handleRemovals();
};