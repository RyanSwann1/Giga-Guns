#pragma once

#include "TileSheetDetails.h"
#include "TileSheet.h"
#include <SFML\Graphics.hpp>
#include <string>

struct Tile
{
	Tile(const sf::Vector2i& position, const TileSheetDetails& tileSheetDetails, const int tileID, sf::Texture& mapTexture)
		: m_position(position),
		m_tileID(tileID)
	{
		m_sprite.setTexture(mapTexture);
		m_sprite.setTextureRect(TileSheet::getTileLocationByID(tileSheetDetails, tileID));
		const int tileSize = tileSheetDetails.m_tileSize;
		m_sprite.setPosition(sf::Vector2f(position.x * tileSize, position.y * tileSize));
	}

	const sf::Vector2i m_position;
	const int m_tileID;
	sf::Sprite m_sprite;
};