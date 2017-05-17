#include "Enemy.h"

Enemy::Enemy(int ID, const sf::Vector2f & position)
	: m_ID(ID),
	m_rect(sf::Vector2f(32, 32))
{
	m_rect.setPosition(position);
	m_rect.setFillColor(sf::Color::Black);
}

const sf::RectangleShape & Enemy::getRect() const
{
	return m_rect;
}

int Enemy::getID() const
{
	return m_ID;
}

void Enemy::draw(sf::RenderWindow & window)
{
	window.draw(m_rect);
}