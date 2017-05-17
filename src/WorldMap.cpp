#include "WorldMap.h"
#include "Base64.h"
#include "XML\tinyxml.h"
#include <algorithm>
#include <assert.h>
#include <utility>
			
//TileSheetDetails
WorldMap::TileSheetDetails::TileSheetDetails(const std::string& name, int tileSize, int rows, int columns, double margin, double spacing)
	: m_name(name), 
	m_tileSize(tileSize),
	m_rows(rows),
	m_columns(columns),
	m_margin(margin),
	m_spacing(spacing)
{}

//TileSheet
WorldMap::TileSheet::TileSheet(const TileSheetDetails& tileSheetDetails, const std::string& tileSheetPath)
	: m_tileSheetDetails(tileSheetDetails),
	m_texture(std::make_unique<sf::Texture>()),
	m_filePath(tileSheetPath)
{
	const bool textureLoaded = m_texture->loadFromFile(tileSheetPath);
	assert(textureLoaded);
}

WorldMap::TileSheet::TileSheet(TileSheet&& orig)
	: m_tileSheetDetails(orig.m_tileSheetDetails),
	m_filePath(orig.m_filePath),
	m_texture(std::move(orig.m_texture))
{
}

const std::string& WorldMap::TileSheet::getFilePath() const
{
	return m_filePath;
}

const WorldMap::TileSheetDetails& WorldMap::TileSheet::getDetails() const
{
	return m_tileSheetDetails;
}

const sf::Texture& WorldMap::TileSheet::getTexture() const
{
	assert(m_texture);
	return *m_texture.get();
}

sf::IntRect WorldMap::TileSheet::getTileLocationByID(int ID) const
{
	//Scan through every tile on tilesheet to find appropriate tile by ID
	int col = 0, row = 0;
	const int firstTileID = 0;
	const int endTileID = m_tileSheetDetails.m_rows * m_tileSheetDetails.m_columns;

	for (int i = firstTileID; i < endTileID; ++i)
	{
		//Appropriate tile has been found
		if (i == ID)
		{
			break;
		}

		//If tile not found, move onto the next tile in the row
		++col;

		//If reaching end of row, move onto the next
		if (col == m_tileSheetDetails.m_columns)
		{
			col = 0;
			++row;
		}
	}

	//Get location on the tile sheet
	const int tileSize = m_tileSheetDetails.m_tileSize;
	const double margin = m_tileSheetDetails.m_margin;
	const double spacing = m_tileSheetDetails.m_spacing;

	return sf::IntRect((margin + ((spacing + tileSize) * col)),
		(margin + ((spacing + tileSize) * row)),
		tileSize, tileSize);
}

sf::IntRect WorldMap::TileSheet::getTileLocationByPosition(const sf::IntRect& position) const
{
	const int tileSize = m_tileSheetDetails.m_tileSize;
	return sf::IntRect(position.left * tileSize, position.top * tileSize, position.width * tileSize, position.height * tileSize);
}

//CollidableTileLayer
void WorldMap::CollidableTileLayer::setTileMap(const std::vector<std::vector<int>>& tileMapData, const LevelDetails& levelDetails)
{
	if (!m_tileMap.empty())
	{
		m_tileMap.clear();
	}

	for (int row = 0; row < levelDetails.m_mapSize.y; ++row)
	{
		for (int col = 0; col < levelDetails.m_mapSize.x; ++col)
		{
			const int tileID = tileMapData[col][row];
			const int tileSize = levelDetails.m_tileSize;
			if (tileID > 0)
			{
				m_tileMap.emplace_back(row * tileSize, col * tileSize, tileSize, tileSize);
			}
		}
	}
}

const std::vector<sf::FloatRect>& WorldMap::CollidableTileLayer::getTileMap() const
{
	return m_tileMap;
}

//TileLayer
WorldMap::TileLayer::Tile::Tile(const sf::Vector2f& position, const TileSheet& tileSheet, int tileID)
	: m_position(position, sf::Vector2f(tileSheet.getDetails().m_tileSize, tileSheet.getDetails().m_tileSize)),
	m_tileID(tileID),
	m_tileSheet(tileSheet)
{
	m_sprite.setTexture(m_tileSheet.getTexture());
	m_sprite.setTextureRect(m_tileSheet.getTileLocationByID(m_tileID));
	const int tileSize = m_tileSheet.getDetails().m_tileSize;
	m_sprite.setPosition(sf::Vector2f(m_position.left * tileSize, m_position.top * tileSize));
}

WorldMap::TileLayer::Tile::Tile(const Tile& orig)
	: m_position(orig.m_position),
	m_tileID(orig.m_tileID),
	m_tileSheet(orig.m_tileSheet)
{
	m_sprite.setTexture(m_tileSheet.getTexture());
	m_sprite.setTextureRect(m_tileSheet.getTileLocationByID(m_tileID));
	const int tileSize = m_tileSheet.getDetails().m_tileSize;
	m_sprite.setPosition(sf::Vector2f(m_position.left * tileSize, m_position.top * tileSize));
}

WorldMap::TileLayer::TileLayer(const std::vector<std::vector<int>>& tileMapData, const sf::Vector2i& mapSize, const TileSheet& tileSheet)
	: m_tileMap()
{
	//Load in tile map
	m_tileMap.reserve(static_cast<size_t>(mapSize.x * mapSize.y));

	for (int row = 0; row < mapSize.y; ++row)
	{
		for (int col = 0; col < mapSize.x; ++col)
		{
			const int tileDrawID = tileMapData[row][col]; //Get ID for tile
			if (tileDrawID > 0)
			{
				m_tileMap.emplace_back(sf::Vector2f(col, row), tileSheet, tileDrawID);
			}
		}
	}
}

void WorldMap::TileLayer::draw(sf::RenderWindow& window)
{
	for (const auto& tile : m_tileMap)
	{
		window.draw(tile.m_sprite);
	}
}

//WorldMap
WorldMap::WorldMap(const std::string& mapName)
	: m_tileLayers(),
	m_tileSheets(),
	m_collidableTileLayer()
{
	//Parse Level
	TiXmlDocument levelDocument;
	const bool isOpen = levelDocument.LoadFile(mapName.c_str());
	assert(isOpen);

	const TiXmlElement& rootNode = *levelDocument.RootElement();
	m_levelDetails = parseLevelDetails(rootNode);
	parseTileSheets(rootNode);
	parseTileMap(rootNode);
}

const WorldMap::CollidableTileLayer& WorldMap::getCollidableTileLayer() const
{
	return m_collidableTileLayer;
}

const WorldMap::EnemySpawnLayer& WorldMap::getEnemySpawnLayer() const
{
	return m_enemySpawnLayer;
}

void WorldMap::draw(sf::RenderWindow & window)
{
	for (auto& tileLayer : m_tileLayers)
	{
		tileLayer.draw(window);
	}
}

const WorldMap::LevelDetails & WorldMap::getLevelDetails() const
{
	return m_levelDetails;
}

void WorldMap::parseTileMap(const TiXmlElement & root)
{
	for (const TiXmlElement* tileLayerNode = root.FirstChildElement(); tileLayerNode != nullptr; tileLayerNode = tileLayerNode->NextSiblingElement())
	{
		if (tileLayerNode->Value() != std::string("layer"))
		{
			continue;
		}

		if (tileLayerNode->Attribute("name") == std::string("Collidable TileLayer"))
		{
			m_collidableTileLayer.setTileMap(decodeTileLayer(*tileLayerNode), m_levelDetails);
			continue;
		}
		else if (tileLayerNode->Attribute("name") == std::string("Enemy Spawn Layer"))
		{
			m_enemySpawnLayer.setMap(decodeTileLayer(*tileLayerNode), m_levelDetails);
			continue;
		}

		//Getting the tile sheet name that the tile layer uses
		//Does mean that tile layer can only use one tile sheet
		//Will have to implement a way to be able to use multiple tilesheets for a singular layer when needed
		const auto& tileSheetPropertyNode = *tileLayerNode->FirstChildElement()->FirstChildElement();
		const std::string tileSheetName = tileSheetPropertyNode.Attribute("value");
		
		m_tileLayers.emplace_back(decodeTileLayer(*tileLayerNode), m_levelDetails.m_mapSize, getTileSheet(tileSheetName));
	}
}

void WorldMap::parseTileSheets(const TiXmlElement & root)
{
	for (const TiXmlElement* tileSetNode = root.FirstChildElement(); tileSetNode != nullptr; tileSetNode = tileSetNode->NextSiblingElement())
	{
		if (tileSetNode->Value() != std::string("tileset"))
		{
			continue;
		}

		const TiXmlElement* const imgRoot = tileSetNode->FirstChildElement("image");
		assert(imgRoot);
		const std::string tileSheetName = tileSetNode->Attribute("name");
		const std::string tileSheetPath = imgRoot->Attribute("source");

		sf::Vector2i pos, tileSetSize;
		tileSetNode->FirstChildElement()->Attribute("width", &tileSetSize.x);
		tileSetNode->FirstChildElement()->Attribute("height", &tileSetSize.y);
		int tileHeight = 0, tileWidth = 0;
		tileSetNode->Attribute("tilewidth", &tileWidth);
		tileSetNode->Attribute("tileheight", &tileHeight);
		assert(tileHeight == tileWidth);
		int tileSize = 0;
		tileSetNode->Attribute("tilewidth", &tileSize);
		double spacing = 0, margin = 0;
		tileSetNode->Attribute("spacing", &spacing);
		tileSetNode->Attribute("margin", &margin);
		const int columns = tileSetSize.x / (tileSize + spacing);
		const int rows = tileSetSize.y / (tileSize + spacing);

		const TileSheetDetails tileSheetDetails(tileSheetName, tileSize, rows, columns, margin, spacing);
		m_tileSheets.emplace(tileSheetName, TileSheet(tileSheetDetails, tileSheetPath));
	}
}

std::vector<std::vector<int>> WorldMap::decodeTileLayer(const TiXmlElement & tileLayerElement) const
{
	std::vector<std::vector<int>> tileData;

	std::string decodedIDs; //Base64 decoded information
	const TiXmlElement* dataNode = nullptr; //Store our node once we find it
	for (const TiXmlElement* e = tileLayerElement.FirstChildElement(); e != nullptr; e = e->NextSiblingElement())
	{
		if (e->Value() == std::string("data"))
		{
			dataNode = e;
		}
	}
	assert(dataNode);

	Base64 base64;
	const TiXmlText* text = dataNode->FirstChild()->ToText();
	const std::string t = text->Value();
	decodedIDs = base64.base64_decode(t);

	const std::vector<int> layerColumns(m_levelDetails.m_mapSize.x);
	for (int i = 0; i < m_levelDetails.m_mapSize.y; ++i)
	{
		tileData.push_back(layerColumns);
	}

	for (int rows = 0; rows < m_levelDetails.m_mapSize.y; ++rows)
	{
		for (int cols = 0; cols < m_levelDetails.m_mapSize.x; ++cols)
		{
			tileData[rows][cols] = *((int*)decodedIDs.data() + rows * m_levelDetails.m_mapSize.x + cols) - 1;
		}
	}

	return tileData;
}

WorldMap::LevelDetails WorldMap::parseLevelDetails(const TiXmlElement & root)
{
	int width = 0, height = 0, tileSize = 0;
	root.Attribute("width", &width);
	root.Attribute("height", &height);
	root.Attribute("tilewidth", &tileSize);

	return WorldMap::LevelDetails(tileSize, sf::Vector2i(width, height));
}

const WorldMap::TileSheet & WorldMap::getTileSheet(const std::string & name) const
{
	auto cIter = m_tileSheets.find(name);
	assert(cIter != m_tileSheets.cend());
	return cIter->second;
}

void WorldMap::EnemySpawnLayer::setMap(const std::vector<std::vector<int>>& tileMapData, const LevelDetails& levelDetails)
{
	for (int row = 0; row < levelDetails.m_mapSize.y; ++row)
	{
		for (int col = 0; col < levelDetails.m_mapSize.x; ++col)
		{
			const int tileID = tileMapData[row][col];
			const int tileSize = levelDetails.m_tileSize;
			if (tileID > 0)
			{
				m_enemySpawnLocations.emplace_back(col * tileSize, row * tileSize);
			}
		}
	}
}

const std::vector<sf::Vector2f>& WorldMap::EnemySpawnLayer::getMap() const
{
	return m_enemySpawnLocations;
}
