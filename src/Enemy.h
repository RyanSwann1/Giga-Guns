#pragma once

#include <SFML\Graphics.hpp>
class Enemy
{
public:
	Enemy(int ID, const sf::Vector2f& position);

	const sf::RectangleShape& getRect() const;
	int getID() const;
	void draw(sf::RenderWindow& window);

private:
	const int m_ID;
	sf::RectangleShape m_rect;
};