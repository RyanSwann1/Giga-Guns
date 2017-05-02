#include "TileLayer.h"
#include "WorldMap.h"
#include <iostream>
#include <utility>
#include <functional>
#include <iostream>

TileLayer::TileLayer(const std::vector<std::vector<int>>& tileMapData, const sf::Vector2i& mapSize, const TileSheetDetails& tileSheetDetails, std::string&& name, sf::Texture& mapTexture)
	: m_name(std::move(name)),
	m_tileMap(),
	m_tileCount(0),
	m_mapTexture(mapTexture)
{
	loadInTileMap(tileMapData, mapSize, tileSheetDetails, mapTexture);
}

void TileLayer::draw(sf::RenderWindow & window)
{
	for (const auto& tile : m_tileMap)
	{
		window.draw(tile.m_sprite);
	}
}

void TileLayer::loadInTileMap(const std::vector<std::vector<int>>& tileMapData, const sf::Vector2i & mapSize, const TileSheetDetails& tileSheetDetails, sf::Texture& mapTexture)
{
	if (!m_tileMap.empty())
	{
		clearMap();
	}

	m_tileMap.reserve(static_cast<size_t>(mapSize.x * mapSize.y));

	for (int col = 0; col < mapSize.y; ++col)
	{
		for (int row = 0; row < mapSize.x; ++row)
		{
			int tileDrawID = tileMapData[col][row]; //Get ID for tile#
			if (tileDrawID > 0)
			{
				//tileDrawID; 
				m_tileMap.emplace_back(sf::Vector2i(row, col), tileSheetDetails, tileDrawID, mapTexture);
			}
		}
	}
}