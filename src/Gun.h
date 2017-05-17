#pragma once

#include <SFML\Graphics.hpp>

enum class Direction;
class Gun
{
public:
	Gun();
	void update(float deltaTime);
	void increaseBulletCount(int quantity);
	bool fire();

private:
	float m_elaspedTime;
	float m_timeBetweenShots;
	int m_bulletCount;
	int m_maxBulletCount;
};