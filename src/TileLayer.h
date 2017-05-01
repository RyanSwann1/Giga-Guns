#pragma once

#include "Tile.h"
#include <vector>
#include <unordered_map>
#include <string>
#include <SFML\Graphics.hpp>
#include <utility>

struct TileSheetDetails;
class TileLayer
{
public:
	TileLayer(const std::vector<std::vector<int>>& tileMapData, const sf::Vector2i& mapSize, 
		const TileSheetDetails& tileSheetDetails, std::string&& name, sf::Texture& mapTexture);

	const std::string& getName() const { return m_name; }
	const std::vector<Tile>& getTileMap() const { return m_tileMap; }

	void draw(sf::RenderWindow& window);

private:
	std::vector<Tile> m_tileMap;
	const std::string m_name;
	sf::Texture& m_mapTexture;
	int m_tileCount;

	void loadInTileMap(const std::vector<std::vector<int>>& tileMapData, const sf::Vector2i& mapSize, const TileSheetDetails& tileSheetDetails, sf::Texture& mapTexture);
	void clearMap() { m_tileMap.clear(); }
};