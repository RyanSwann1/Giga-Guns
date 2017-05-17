#include "GunManager.h"
#include "CollisionHandler.h"
#include "EnemyManager.h"
#include <algorithm>
#include <assert.h>

//Bullet
GunManager::Bullet::Bullet(GunManager& gunManager, const sf::Vector2f & spawnPosition, Direction moveDirection, int ID)
	: m_movementSpeed(250),
	m_movementDirection(moveDirection),
	m_ID(ID),
	m_gunManager(gunManager),
	m_rect(sf::Vector2f(10, 10))
{
	m_rect.setPosition(spawnPosition);
	m_rect.setFillColor(sf::Color::Black);
}

int GunManager::Bullet::getID() const
{
	return m_ID;
}

const sf::Vector2f & GunManager::Bullet::getPosition() const
{
	return m_rect.getPosition();
}

void GunManager::Bullet::update(EnemyManager& enemyManager, const WorldMap& worldMap, float deltaTime)
{
	auto position = m_rect.getPosition();
	switch (m_movementDirection)
	{
	case Direction::Left :
	{
		position.x -= m_movementSpeed * deltaTime;
		break;
	}
	case Direction::Right : 
	{
		position.x += m_movementSpeed * deltaTime;
		break;
	}
	case Direction::Up :
	{
		position.y -= m_movementSpeed * deltaTime;
		break;
	}
	case Direction::Down :
	{
		position.y += m_movementSpeed * deltaTime;
		break;
	}
	}

	m_rect.setPosition(position);
	handleCollisions(enemyManager, worldMap);
}

void GunManager::Bullet::draw(sf::RenderWindow & window)
{
	window.draw(m_rect);
}

void GunManager::Bullet::handleCollisions(EnemyManager& enemyManager, const WorldMap & worldMap)
{
	if (CollisionHandler::tileCollision(sf::FloatRect(m_rect.getPosition(), m_rect.getSize()), worldMap))
	{
		m_gunManager.removeBullet(m_ID);
	}

	const sf::FloatRect bulletAABB(m_rect.getPosition(), m_rect.getSize());
	const Enemy* enemy = CollisionHandler::entityCollision(sf::FloatRect(bulletAABB), enemyManager);
	if (enemy)
	{
		enemyManager.removeEnemy(enemy->getID());
		m_gunManager.removeBullet(m_ID);
	}

	if (CollisionHandler::entityOutOfBounds(sf::FloatRect(m_rect.getPosition(), m_rect.getSize()), worldMap))
	{
		m_gunManager.removeBullet(m_ID);
	}
}

//GunManager
GunManager::GunManager()
	: m_guns(),
	m_bullets(),
	m_equippedGun(nullptr),
	m_bulletID(0)
{
	m_guns.emplace_back();
	m_equippedGun = &m_guns.back();
}

void GunManager::update(EnemyManager& enemyManager, const WorldMap& worldMap, float deltaTime)
{
	for (auto& gun : m_guns)
	{
		gun.update(deltaTime);
	}

	for (auto& bullet : m_bullets)
	{
		bullet->update(enemyManager, worldMap, deltaTime);
	}

	handleRemovals();
}

void GunManager::draw(sf::RenderWindow & window)
{
	for (auto& bullet : m_bullets)
	{
		bullet->draw(window);
	}
}

void GunManager::removeBullet(int ID)
{
	if (std::find(m_bulletRemovals.cbegin(), m_bulletRemovals.cend(), ID) == m_bulletRemovals.cend())
	{
		m_bulletRemovals.push_back(ID);
	}
}

void GunManager::handleRemovals()
{
	for (auto& bulletID : m_bulletRemovals)
	{
		auto iter = std::find_if(m_bullets.begin(), m_bullets.end(), [bulletID](const auto& bullet) {return bullet->getID() == bulletID; });
		assert(iter != m_bullets.cend());
		m_bullets.erase(iter);
	}

	m_bulletRemovals.clear();
}

void GunManager::fireEquippedGun(const sf::Vector2f & spawnPosition, Direction moveDirection)
{
	if (!m_equippedGun)
	{
		return;
	}

	if (m_equippedGun->fire())
	{
		m_bullets.emplace_back(std::make_unique<Bullet>(*this, spawnPosition, moveDirection, m_bulletID));
		++m_bulletID;
	}
}