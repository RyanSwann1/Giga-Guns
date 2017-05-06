#include "WorldMap.h"
#include "Base64.h"
#include "XML\tinyxml.h"
#include <algorithm>
#include <assert.h>
#include <utility>
#include <SFML\Window.hpp>
#include <SFML\Graphics.hpp>

struct LevelDetails
{
	LevelDetails(int tileSize, const sf::Vector2i& mapSize)
		: m_tileSize(tileSize),
		m_mapSize(mapSize)
	{}

	const int m_tileSize;
	const sf::Vector2i m_mapSize;
};

LevelDetails parseLevelDetails(const TiXmlElement& root);

//TileSheetDetails.cpp
WorldMap::TileSheetDetails::TileSheetDetails(std::string&& name, int tileSize, int rows, int columns, double margin, double spacing)
	: m_name(std::move(name)), 
	m_tileSize(tileSize),
	m_rows(rows),
	m_columns(columns),
	m_margin(margin),
	m_spacing(spacing)
{}

//TileSheet.cpp
WorldMap::TileSheet::TileSheet(const TileSheetDetails& tileSheetDetails, const std::string& tileSheetPath)
	: m_tileSheetDetails(tileSheetDetails),
	m_texture(new sf::Texture()),
	m_filePath(tileSheetPath)
{
	const bool textureLoaded = m_texture->loadFromFile(tileSheetPath);
	assert(textureLoaded);
}

WorldMap::TileSheet::~TileSheet()
{
	delete m_texture;
	m_texture = nullptr;
}

WorldMap::TileSheet::TileSheet(const TileSheet& orig)
	: m_tileSheetDetails(orig.m_tileSheetDetails),
	m_filePath(orig.m_filePath)
{
	m_texture = new sf::Texture(*orig.m_texture);
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
	return *m_texture;
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

//TileLayer.cpp
WorldMap::TileLayer::Tile::Tile(const sf::Vector2i& position, const TileSheet& tileSheet, int tileID)
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

WorldMap::TileLayer::TileLayer(const std::vector<std::vector<int>>& tileMapData, const sf::Vector2i& mapSize, const TileSheet& tileSheet, std::string&& name)
	: m_name(std::move(name)),
	m_tileMap()
{
	loadInTileMap(tileMapData, mapSize, tileSheet);
}

void WorldMap::TileLayer::loadInTileMap(const std::vector<std::vector<int>>& tileMapData, const sf::Vector2i& mapSize, const TileSheet& tileSheet)
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
			int tileDrawID = tileMapData[col][row]; //Get ID for tile
			if (tileDrawID > 0)
			{
				m_tileMap.emplace_back(sf::Vector2i(row, col), tileSheet, tileDrawID);
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

void WorldMap::TileLayer::clearMap()
{
	m_tileMap.clear();
}

const std::string& WorldMap::TileLayer::getName() const
{
	return m_name;
}

//WorldMap.cpp
WorldMap::WorldMap(const std::string& mapName)
	: m_tileLayers()
{
	parseLevel(mapName);
}

WorldMap::~WorldMap()
{
}

void WorldMap::assignTileLayer(const std::vector<std::vector<int>>& tileData, const sf::Vector2i & mapSize, const std::string& tileSheetName, std::string&& tileLayerName)
{
	assert(!hasTileLayer(tileLayerName));
	m_tileLayers.emplace_back(tileData, mapSize, getTileSheet(tileSheetName), std::move(tileLayerName));
}

bool WorldMap::hasTileLayer(const std::string& tileLayerName) const
{
	return (std::find_if(m_tileLayers.cbegin(), m_tileLayers.cend(),
		[&tileLayerName](auto& tileLayer) { return tileLayer.getName() == tileLayerName; }) != m_tileLayers.cend() ? true : false);
}

bool WorldMap::hasTileSheet(const std::string & tileSheetName) const
{
	auto cIter = m_tileSheets.find(tileSheetName);
	return (cIter != m_tileSheets.cend() ? false : true);
}

void WorldMap::parseLevel(const std::string & mapName)
{
	//Load XML file
	TiXmlDocument levelDocument;
	const bool isOpen = levelDocument.LoadFile(mapName.c_str());
	assert(isOpen);

	const TiXmlElement& rootNode = *levelDocument.RootElement();
	const LevelDetails levelDetails = parseLevelDetails(rootNode);
	parseTileSheets(rootNode);
	parseTileMap(rootNode, levelDetails);
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

void WorldMap::addTileSheet(const std::string& tileSheetPath, const TileSheetDetails & tileSheetDetails)
{
	assert(!hasTileLayer(tileSheetDetails.m_name));
	m_tileSheets.emplace(tileSheetDetails.m_name, TileSheet(tileSheetDetails, tileSheetPath));
}

void WorldMap::parseTileMap(const TiXmlElement & root, const LevelDetails & levelDetails)
{
	const int tileLayerCount = getNumberOfTileLayers(root);
	const TiXmlElement* tileLayerNode = findNode(root, "layer"); //Get the first layer node
	assert(tileLayerNode);
	for (int i = 1; i <= tileLayerCount; ++i)
	{
		//If node in question is a layer 
		if (tileLayerNode->Value() == std::string("layer") &&
			tileLayerNode->Attribute("name") == std::string("Tile Layer " + std::to_string(i)))
		{
			std::string tileLayerName = tileLayerNode->Attribute("name");
			//assert(tileLayerNode->Attribute("tilesheetname"));
			std::string tileSheetName = "tilesheet_complete";

			if (tileLayerNode->FirstChildElement()->Value() == std::string("data") ||
				tileLayerNode->FirstChildElement()->NextSiblingElement() != 0 && tileLayerNode->FirstChildElement()->NextSiblingElement()->Value() == std::string("data"))
			{
				assignTileLayer(decodeTileLayer(*tileLayerNode, levelDetails), levelDetails.m_mapSize, tileSheetName, std::move(tileLayerName));
			}
		}
		if (tileLayerNode->NextSiblingElement())
		{
			//Go to the next layer
			tileLayerNode = tileLayerNode->NextSiblingElement();
		}
	}
}

void WorldMap::parseTileSheets(const TiXmlElement & root)
{
	const TiXmlElement* tileSetNode = findNode(root, "tileset");
	assert(tileSetNode);
	bool continueSearching = true;
	while (continueSearching)
	{
		if (tileSetNode->Value() == std::string("tileset") && tileSetNode->Attribute("name") != std::string("Collidable"))
		{
			//Register the tile set texture
			const TiXmlElement* const imgRoot = tileSetNode->FirstChildElement("image");
			assert(imgRoot);
			std::string tileSheetName;
			assert(tileSetNode->Attribute("name"));
			tileSheetName = tileSetNode->Attribute("name");
			std::string tileSheetPath;
			assert(imgRoot->Attribute("source"));
			tileSheetPath = imgRoot->Attribute("source");

			//Load in values from file
			sf::Vector2i pos, tileSetSize;
			tileSetNode->FirstChildElement()->Attribute("width", &tileSetSize.x);
			tileSetNode->FirstChildElement()->Attribute("height", &tileSetSize.y);
			int firstGridID = 0, tileSize = 0;
			tileSetNode->Attribute("firstgid", &firstGridID);
			//assert(tileSetNode->Attribute("tileWidth") == tileSetNode->Attribute("tileheight"));
			tileSetNode->Attribute("tilewidth", &tileSize);
			double spacing = 0, margin = 0;
			tileSetNode->Attribute("spacing", &spacing);
			tileSetNode->Attribute("margin", &margin);
			const int columns = tileSetSize.x /(tileSize + spacing);
			const int rows = tileSetSize.y / (tileSize + spacing);

			//void WorldMap::addTileSheet(const TileSheetDetails & tileSheetDetails, std::string && tileSheetPath)
			addTileSheet(std::move(tileSheetPath), TileSheetDetails(std::move(tileSheetName), tileSize, rows, columns, margin, spacing));
		}

		//Change to the next tileset
		if (tileSetNode->NextSiblingElement())
		{
			tileSetNode = tileSetNode->NextSiblingElement();
		}
		else
		{
			continueSearching = false;
		}
	}
}

LevelDetails parseLevelDetails(const TiXmlElement & root)
{
	int width = 0, height = 0, tileSize = 0;
	root.Attribute("width", &width);
	root.Attribute("height", &height);
	root.Attribute("tilewidth", &tileSize);

	return LevelDetails(tileSize, sf::Vector2i(width, height));
}

std::vector<std::vector<int>> WorldMap::decodeTileLayer(const TiXmlElement & tileLayerElement, const LevelDetails & levelDetails) const
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

	Base64 base64;
	//Get the text from within the node and use base64 to decode it
	//assert data node for more children
	for (const TiXmlNode* e = dataNode->FirstChild(); e != nullptr; e = e->NextSibling())
	{
		const TiXmlText* text = e->ToText();
		std::string t = text->Value();
		decodedIDs = base64.base64_decode(t);
	}

	std::vector<int> layerRow(levelDetails.m_mapSize.x);
	//resrerve
	for (int i = 0; i < levelDetails.m_mapSize.y; ++i)
	{
		tileData.push_back(layerRow);
	}

	for (int rows = 0; rows < levelDetails.m_mapSize.y; ++rows)
	{
		for (int cols = 0; cols < levelDetails.m_mapSize.x; ++cols)
		{
			tileData[rows][cols] = *((int*)decodedIDs.data() + rows * levelDetails.m_mapSize.x + cols) - 1;

		}
	}

	return std::move(tileData);
}

const TiXmlElement * WorldMap::findNode(const TiXmlElement & root, const std::string & name) const
{
	for (const TiXmlElement* e = root.FirstChildElement(); e != nullptr; e = e->NextSiblingElement())
	{
		if (e->Value() == name)
		{
			return e;
		}
	}
	return nullptr;
}

const TiXmlElement * WorldMap::findNode(const TiXmlElement & root, const std::string & value, const std::string & name) const
{
	for (const TiXmlElement* e = root.FirstChildElement(); e != nullptr; e = e->NextSiblingElement())
	{
		if (e->Value() == value && e->Attribute("name") == name)
		{
			return e;
		}
	}

	return nullptr;
}

int WorldMap::getNumberOfTileLayers(const TiXmlElement & root) const
{
	int tileLayerCount = 0;
	for (const TiXmlElement* e = root.FirstChildElement(); e != nullptr; e = e->NextSiblingElement())
	{
		if (e->Value() == std::string("layer"))
		{
			++tileLayerCount;
		}
	}
	return tileLayerCount;
}

int WorldMap::getNumberOfTileSets(const TiXmlElement & root) const
{
	int tileSetCount = 0;
	const TiXmlElement* tileSetNode = findNode(root, "tileset");
	assert(tileSetNode);
	for (const TiXmlElement* e = tileSetNode; e != nullptr; e = e->NextSiblingElement())
	{
		if (e->Value() == std::string("tileset") && e->Attribute("name") != std::string("Collidable"))
		{
			++tileSetCount;
		}
	}
	return tileSetCount;
}

const WorldMap::TileSheet & WorldMap::getTileSheet(const std::string & name) const
{
	auto cIter = m_tileSheets.find(name);
	assert(cIter != m_tileSheets.cend());
	return cIter->second;
}
