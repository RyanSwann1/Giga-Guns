#pragma once

#include "TileLayer.h"
#include "TileSheetDetails.h"
#include <vector>
#include <SFML\Graphics.hpp>
#include <string>

class WorldMap
{
public:
	WorldMap();
	WorldMap(const WorldMap&) = delete;
	WorldMap& operator=(const WorldMap&) = delete;

	void assignTileLayer(const std::vector<std::vector<int>>& tileData, const sf::Vector2i& mapSize, std::string&& tileLayerName);
	bool hasTileLayer(const std::string& tileLayerName) const;
	void draw(sf::RenderWindow& window);

private:
	std::vector<TileLayer> m_tileLayers;
	std::string m_currentMapName;
	sf::Texture m_mapTexture;
	TileSheetDetails m_tileSheetDetails;

	void clearMap();
};