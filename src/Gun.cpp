#include "Gun.h"

Gun::Gun()
	: m_elaspedTime(0),
	m_timeBetweenShots(0.75),
	m_bulletCount(50),
	m_maxBulletCount(50)
{
}

void Gun::update(float deltaTime)
{
	m_elaspedTime += deltaTime;
}

void Gun::increaseBulletCount(int quantity)
{
	m_bulletCount += quantity;
	if (m_bulletCount > m_maxBulletCount)
	{
		m_bulletCount = m_maxBulletCount;
	}
}

bool Gun::fire()
{
	if (m_elaspedTime >= m_timeBetweenShots && m_bulletCount)
	{
		--m_bulletCount;
		m_elaspedTime = 0;
		return true;
	}

	return false;
}
