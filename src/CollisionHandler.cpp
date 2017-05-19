#include "CollisionHandler.h"
#include "WorldMap.h"
#include <math.h>

void CollisionHandler::clampMovement(sf::Vector2f& movement, const sf::Vector2f& position, const sf::Vector2f& size, const WorldMap & worldMap)
{
	const float tileSize = worldMap.getLevelDetails().m_tileSize;
	const sf::Vector2f tileDimensions = { tileSize, tileSize };
	sf::FloatRect entityAABB(position + movement, size);
	for (const auto& tile : worldMap.getCollidableTileLayer().getTileMap())
	{
		sf::FloatRect tileAABB(sf::Vector2f{ tile }, tileDimensions);
		sf::FloatRect intersection;
		if (entityAABB.intersects(tileAABB, intersection))
		{
			if (movement.x < 0.f)
			{
				movement.x += intersection.width;
			}
			else if (movement.x > 0.f)
			{
				movement.x -= intersection.width;
			}
			if (movement.y < 0.f)
			{
				movement.y += intersection.height;
			}
			else if (movement.y > 0.f)
			{
				movement.y -= intersection.height;
			}
			break;
		}
	}
}