#include "WorldMap.h"
#include <iostream>
#include <algorithm>
#include <fstream>
#include <sstream>
#include <assert.h>
#include <functional>

WorldMap::WorldMap()
	: m_tileLayers(),
	m_currentMapName(),
	m_tileSheetDetails(16, 53, 54, 0, 0)
{
	m_mapTexture.loadFromFile("tilesheet_complete.PNG");
}

void WorldMap::assignTileLayer(const std::vector<std::vector<int>>& tileData, const sf::Vector2i & mapSize, std::string&& tileLayerName)
{
	if (!hasTileLayer(tileLayerName))
	{
		m_tileLayers.emplace_back(tileData, mapSize, m_tileSheetDetails, std::move(tileLayerName), m_mapTexture);
	}
}

bool WorldMap::hasTileLayer(const std::string& tileLayerName) const
{
	return (std::find_if(m_tileLayers.cbegin(), m_tileLayers.cend(),
		[&tileLayerName](auto& tileLayer) { return tileLayer.getName() == tileLayerName; }) != m_tileLayers.cend() ? true : false);
}

void WorldMap::draw(sf::RenderWindow & window)
{
	for (auto& tileLayer : m_tileLayers)
	{
		tileLayer.draw(window);
	}
}

void WorldMap::clearMap()
{
	m_tileLayers.clear();
}
