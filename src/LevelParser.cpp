#include "LevelParser.h"
#include "zlib.h"
#include "XML\tinystr.h"
#include "XML\tinyxml.h"
#include "WorldMap.h"
#include "Base64.h"
#include <assert.h>
#include <iostream>
#include <fstream>
#include <algorithm>
#include <vector>
#include <SFML\Graphics.hpp>
#include <utility>

struct LevelDetails
{
	LevelDetails(const int tileSize, const sf::Vector2i& mapSize)
		: m_tileSize(tileSize),
		m_mapSize(mapSize)
	{}

	const int m_tileSize;
	const sf::Vector2i m_mapSize;
};

void parseTileMap(WorldMap& worldMap, const TiXmlElement & root, const LevelDetails & levelDetails);
void parseTileLayer(WorldMap& worldMap, const TiXmlElement & tileLayerElement, const LevelDetails& levelDetails, std::string&& tileLayerName);
LevelDetails parseLevelDetails(const TiXmlElement& root);
std::vector<std::vector<int>> decodeTileLayer(const TiXmlElement & tileLayerElement, const LevelDetails & levelDetails);
const TiXmlElement* findNode(const TiXmlElement& root, const std::string& name);
const TiXmlElement* findNode(const TiXmlElement& root, const std::string& value, const std::string& name);
int getNumberOfTileLayers(const TiXmlElement& root);
int getNumberOfTileSets(const TiXmlElement& root);

void LevelParser::parseLevel(WorldMap& worldMap, const std::string & levelName)
{
	//Load XML file
	TiXmlDocument levelDocument;
	const bool isOpen = levelDocument.LoadFile(levelName.c_str());
	assert(isOpen);

	const TiXmlElement& rootNode = *levelDocument.RootElement();
	const LevelDetails levelDetails = parseLevelDetails(rootNode);
	parseTileMap(worldMap, rootNode, levelDetails);
}

void parseTileLayer(WorldMap& worldMap, const TiXmlElement & tileLayerElement, const LevelDetails& levelDetails, std::string&& tileLayerName)
{
	worldMap.assignTileLayer(decodeTileLayer(tileLayerElement, levelDetails), levelDetails.m_mapSize, std::move(tileLayerName));
}

LevelDetails parseLevelDetails(const TiXmlElement & root)
{
	int width = 0, height = 0, tileSize = 0;
	root.Attribute("width", &width);
	root.Attribute("height", &height);
	root.Attribute("tilewidth", &tileSize);

	return LevelDetails(tileSize, sf::Vector2i(width, height));
}

const TiXmlElement * findNode(const TiXmlElement& root, const std::string & name)
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

const TiXmlElement *findNode(const TiXmlElement & root, const std::string & value, const std::string & name)
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

std::vector<std::vector<int>> decodeTileLayer(const TiXmlElement & tileLayerElement, const LevelDetails & levelDetails)
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
	for (const TiXmlNode* e = dataNode->FirstChild(); e != nullptr; e = e->NextSibling())
	{
		const TiXmlText* text = e->ToText();
		std::string t = text->Value();
		decodedIDs = base64.base64_decode(t);
	}

	std::vector<int> layerRow(levelDetails.m_mapSize.x);
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


int getNumberOfTileLayers(const TiXmlElement & root)
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

int getNumberOfTileSets(const TiXmlElement & root)
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

void parseTileMap(WorldMap& worldMap, const TiXmlElement & root, const LevelDetails & levelDetails)
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

			if (tileLayerNode->FirstChildElement()->Value() == std::string("data") ||
				tileLayerNode->FirstChildElement()->NextSiblingElement() != 0 && tileLayerNode->FirstChildElement()->NextSiblingElement()->Value() == std::string("data"))
			{
				if (!worldMap.hasTileLayer(tileLayerName))
				{
					parseTileLayer(worldMap, *tileLayerNode, levelDetails, std::move(tileLayerName));
				}
			}
		}
		if (tileLayerNode->NextSiblingElement())
		{
			//Go to the next layer
			tileLayerNode = tileLayerNode->NextSiblingElement();
		}
	}
}


