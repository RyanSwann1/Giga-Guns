#include "WorldMap.h"
#include "Base64.h"
#include "XML\tinyxml.h"
#include <algorithm>
#include <assert.h>
#include <utility>
#include <iostream>

//TileSheetDetails
WorldMap::TileSheetDetails::TileSheetDetails(const std::string& name, int tileSize, int rows, int columns, int firstGID, double margin, double spacing)
	: m_name(name), 
	m_tileSize(tileSize),
	m_rows(rows),
	m_columns(columns),
	m_firstGID(firstGID),
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
	const int margin = m_tileSheetDetails.m_margin;
	const int spacing = m_tileSheetDetails.m_spacing;

	return sf::IntRect((margin + ((spacing + tileSize) * col)),
		(margin + ((spacing + tileSize) * row)),
		tileSize, tileSize);
}

sf::IntRect WorldMap::TileSheet::getTileLocationByPosition(const sf::IntRect& position) const
{
	const int tileSize = m_tileSheetDetails.m_tileSize;
	return sf::IntRect(position.left * tileSize, position.top * tileSize, position.width * tileSize, position.height * tileSize);
}

void WorldMap::CollidableTileLayer::addCollidableTile(const sf::Vector2i & tilePosition, int tileSize)
{
	m_tileMap.emplace_back(tilePosition.x, tilePosition.y - 32);
}

const std::vector<sf::Vector2i>& WorldMap::CollidableTileLayer::getTileMap() const
{
	return m_tileMap;
}

//TileLayer
WorldMap::TileLayer::Tile::Tile(const sf::Vector2f& position, const TileSheet& tileSheet, int tileID)
	: m_position(position),
	m_tileID(tileID),
	m_tileSheet(tileSheet)
{
	m_sprite.setTexture(m_tileSheet.getTexture());
	m_sprite.setTextureRect(m_tileSheet.getTileLocationByID(m_tileID));
	const int tileSize = m_tileSheet.getDetails().m_tileSize;
	m_sprite.setPosition(sf::Vector2f(m_position.x * tileSize, m_position.y * tileSize));
}

WorldMap::TileLayer::Tile::Tile(const Tile& orig)
	: m_position(orig.m_position),
	m_tileID(orig.m_tileID),
	m_tileSheet(orig.m_tileSheet)
{
	m_sprite.setTexture(m_tileSheet.getTexture());
	m_sprite.setTextureRect(m_tileSheet.getTileLocationByID(m_tileID));
	const int tileSize = m_tileSheet.getDetails().m_tileSize;
	m_sprite.setPosition(sf::Vector2f(m_position.x * tileSize, m_position.y * tileSize));
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
			const int tileDrawID = tileMapData[row][col];
			if (tileDrawID >= 0)
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
	parseCollidableLayer(rootNode);
}

const WorldMap::CollidableTileLayer& WorldMap::getCollidableTileLayer() const
{
	return m_collidableTileLayer;
}

const WorldMap::LevelDetails & WorldMap::getLevelDetails() const
{
	return m_levelDetails;
}

void WorldMap::draw(sf::RenderWindow & window)
{
	for (auto& tileLayer : m_tileLayers)
	{
		tileLayer.draw(window);
	}
}

void WorldMap::parseTileMap(const TiXmlElement & root)
{
	for (const TiXmlElement* tileLayerNode = root.FirstChildElement(); tileLayerNode != nullptr; tileLayerNode = tileLayerNode->NextSiblingElement())
	{
		if (tileLayerNode->Value() != std::string("layer"))
		{
			continue;
		}

		const auto& tileSheetPropertyNode = *tileLayerNode->FirstChildElement()->FirstChildElement();
		const std::string tileSheetName = tileSheetPropertyNode.Attribute("value");
		m_tileLayers.emplace_back(decodeTileLayer(*tileLayerNode, tileSheetName), m_levelDetails.m_mapSize, getTileSheet(tileSheetName));
	}
}

void WorldMap::parseCollidableLayer(const TiXmlElement & root)
{
	for (const auto* objectLayerNode = root.FirstChildElement(); objectLayerNode != nullptr; 
		objectLayerNode = objectLayerNode->NextSiblingElement())
	{
		if (objectLayerNode->Value() != std::string("objectgroup"))
		{
			continue;
		}

		for (const auto* collidableTileNode = objectLayerNode->FirstChildElement(); collidableTileNode != nullptr;
			collidableTileNode = collidableTileNode->NextSiblingElement())
		{
			sf::Vector2i tilePosition;
			int tileSize = 0;
			collidableTileNode->Attribute("x", &tilePosition.x);
			collidableTileNode->Attribute("y", &tilePosition.y);
			collidableTileNode->Attribute("height", &tileSize);
			m_collidableTileLayer.addCollidableTile(tilePosition, tileSize);
		}

		//Found Collidable tile layer
		break;
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
		int firstGID = 0;
		tileSetNode->Attribute("firstgid", &firstGID);
		double spacing = 0, margin = 0;
		tileSetNode->Attribute("spacing", &spacing);
		tileSetNode->Attribute("margin", &margin);
		const int columns = tileSetSize.x / (tileSize + spacing);
		const int rows = tileSetSize.y / (tileSize + spacing);

		const TileSheetDetails tileSheetDetails(tileSheetName, tileSize, rows, columns, firstGID, margin, spacing);
		m_tileSheets.emplace(tileSheetName, TileSheet(tileSheetDetails, tileSheetPath));
	}
}

std::vector<std::vector<int>> WorldMap::decodeTileLayer(const TiXmlElement & tileLayerElement, const std::string& tileSheetName) const
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

	const auto& tileSheetDetails = getTileSheet(tileSheetName).getDetails();
	for (int rows = 0; rows < m_levelDetails.m_mapSize.y; ++rows)
	{
		for (int cols = 0; cols < m_levelDetails.m_mapSize.x; ++cols)
		{
			tileData[rows][cols] = *((int*)decodedIDs.data() + rows * m_levelDetails.m_mapSize.x + cols) - tileSheetDetails.m_firstGID;
		}
	}

	return tileData;
}

const WorldMap::TileSheet & WorldMap::getTileSheet(const std::string & name) const
{
	auto cIter = m_tileSheets.find(name);
	assert(cIter != m_tileSheets.cend());
	return cIter->second;
}

WorldMap::LevelDetails WorldMap::parseLevelDetails(const TiXmlElement & root) const 
{
	int width = 0, height = 0, tileSize = 0;
	root.Attribute("width", &width);
	root.Attribute("height", &height);
	root.Attribute("tilewidth", &tileSize);

	return LevelDetails(tileSize, sf::Vector2i(width, height));
}
