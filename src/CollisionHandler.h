#pragma once

#include <SFML\Graphics.hpp>
class WorldMap;
namespace CollisionHandler
{
	sf::Vector2f handleTileCollision(const sf::FloatRect& entityAABB, const WorldMap& worldMap);
}