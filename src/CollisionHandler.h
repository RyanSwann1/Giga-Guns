#pragma once

#include <SFML\Graphics.hpp>
class WorldMap;
namespace CollisionHandler
{
	void clampMovement(sf::Vector2f& movement, const sf::Vector2f& position, const sf::Vector2f& size, const WorldMap& worldMap);
}