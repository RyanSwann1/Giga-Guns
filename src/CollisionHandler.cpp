#include "CollisionHandler.h"
#include "WorldMap.h"
#include "EnemyManager.h"
#include <math.h>

sf::Vector2f CollisionHandler::handleTileCollision(const sf::FloatRect & entityAABB, const WorldMap & worldMap)
{
	sf::Vector2f newPos;
	for (const auto& tileAABB : worldMap.getCollidableTileLayer().getTileMap())
	{
		sf::FloatRect intersection;
		if (!entityAABB.intersects(tileAABB, intersection))
		{
			continue;
		}

		//Determine which axis collisions happens
		const float xDifference = (entityAABB.left + (entityAABB.width / 2.0f)) - (tileAABB.left + (tileAABB.width / 2.0f));
		const float yDifference = (entityAABB.top + (entityAABB.height / 2.0f)) - (tileAABB.top + (tileAABB.height / 2.0f));
		if (std::abs(xDifference) > std::abs(yDifference))
		{
			//Determine direction in which to move entity
			if (xDifference > 0.0f)
			{
				newPos.x = intersection.width;
			}
			else
			{
				newPos.x = -intersection.width;
			}
		}
		else
		{
			if (yDifference > 0.0f)
			{
				newPos.y = intersection.height;
			}
			else
			{
				newPos.y = -intersection.height;
			}
		}
	}

	return newPos;
}

bool CollisionHandler::tileCollision(const sf::FloatRect & AABB, const WorldMap& worldMap)
{
	for (const auto& tile : worldMap.getCollidableTileLayer().getTileMap())
	{
		if (AABB.intersects(tile))
		{
			return true;
		}
	}

	return false;
}

const Enemy* CollisionHandler::entityCollision(const sf::FloatRect & AABB, const EnemyManager & enemyManager)
{
	for (const auto& enemy : enemyManager.getEnemies())
	{
		const sf::FloatRect enemyAABB(enemy->getRect().getPosition(), enemy->getRect().getSize());
		if (AABB.intersects(enemyAABB))
		{
			return enemy.get();
		}
	}

	return nullptr;
}

bool CollisionHandler::entityOutOfBounds(const sf::FloatRect& entityAABB, const WorldMap & worldMap)
{
	const auto& mapSize = worldMap.getLevelDetails().m_mapSize;
	const int tileSize = worldMap.getLevelDetails().m_tileSize;
	if (entityAABB.left + entityAABB.width < 0)
	{
		return true;
	}
	else if (entityAABB.top + entityAABB.height < 0)
	{
		return true;
	}
	else if (entityAABB.left > (mapSize.x * tileSize))
	{
		return true;
	}
	else if (entityAABB.top > (mapSize.y * tileSize))
	{
		return true;
	}

	return false;
}
