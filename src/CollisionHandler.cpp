#include "CollisionHandler.h"
#include "WorldMap.h"
#include <math.h>

void CollisionHandler::clampMovement(sf::Vector2f& movement, const sf::Vector2f& position, const sf::Vector2f& size, const WorldMap & worldMap)
{
	const sf::FloatRect entityAABB(position + movement, size);
	for (const auto& tilePosition : worldMap.getCollidableTileLayer().getTileMap())
	{
		sf::FloatRect intersection;
		const int tileSize = worldMap.getLevelDetails().m_tileSize;
		const sf::FloatRect tileAABB(sf::Vector2f(tilePosition.x, tilePosition.y), sf::Vector2f(tileSize, tileSize));
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
				movement.x += intersection.width;
			}
			else
			{
				movement.x -= intersection.width;
			}
		}
		else
		{
			if (yDifference > 0.0f)
			{
				movement.y += intersection.height;
			}
			else
			{
				movement.y -= intersection.height;
			}
		}
		break;
	}
}