#pragma once

#include <SFML\Graphics.hpp>
class WorldMap;
class EnemyManager;
class Enemy;
namespace CollisionHandler
{
	sf::Vector2f handleTileCollision(const sf::FloatRect& entityAABB, const WorldMap& worldMap);
	bool tileCollision(const sf::FloatRect& AABB, const WorldMap& worldMap);
	const Enemy* entityCollision(const sf::FloatRect& AABB, const EnemyManager& enemyManager);
	bool entityOutOfBounds(const sf::FloatRect& entityAABB, const WorldMap& worldMap);
}