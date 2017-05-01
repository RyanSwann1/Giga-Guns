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
const TiXmlElement* findNode(const TiXmlElement& root, const std::string& name);
const TiXmlElement* findNode(const TiXmlElement& root, const std::string& value, const std::string& name);
std::vector<std::vector<int>> decodeTileLayer(const TiXmlElement& tileLayerElement, const LevelDetails& levelDetails);
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

	uLongf sizeOfIDs = levelDetails.m_mapSize.x * levelDetails.m_mapSize.y * sizeof(int);
	std::vector<int> ids(levelDetails.m_mapSize.x * levelDetails.m_mapSize.y);

	uncompress((Bytef*)&ids[0], &sizeOfIDs, (const Bytef*)decodedIDs.c_str(), decodedIDs.size());

	std::vector<int> layerRow(levelDetails.m_mapSize.x);
	for (int i = 0; i < levelDetails.m_mapSize.y; ++i)
	{
		tileData.push_back(layerRow);
	}

	for (int rows = 0; rows < levelDetails.m_mapSize.y; ++rows)
	{
		for (int cols = 0; cols < levelDetails.m_mapSize.x; ++cols)
		{
			tileData[rows][cols] = ids[rows * levelDetails.m_mapSize.x + cols];
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
					//Find correct tilesheet that corresponds with this tile layer
					if (tileLayerNode->FirstChildElement()->Value() == std::string("properties"))
					{
						const TiXmlElement& tileLayerProperty = *tileLayerNode->FirstChildElement()->FirstChildElement();
						const std::string tileSheetName = tileLayerProperty.Attribute("value");

						parseTileLayer(worldMap, *tileLayerNode, levelDetails, std::move(tileLayerName));
					}

					////If only one tile set exists
					//if (tileSets.size() == static_cast<size_t>(1))
					//{
					//	std::cout << tileLayerName << "\n";
					//	parseTileLayer(*tileLayerNode, levelDetails, *tileSets.begin(), tileLayerName);
					//}
					////If more than one tile set exists
					//else
					//{
					//	if (tileLayerNode->FirstChildElement()->Value() == std::string("properties"))
					//	{
					//		const TiXmlElement& tileLayerProperty = *tileLayerNode->FirstChildElement()->FirstChildElement();
					//		const std::string tileSetName = tileLayerProperty.Attribute("value");
					//		auto cIter = std::find_if(tileSets.cbegin(), tileSets.cend(), [tileSetName](const TileSetDetails& tileSetDetails) {return tileSetDetails.m_name == tileSetName; });
					//		if (cIter != tileSets.cend())
					//		{
					//			std::cout << "Tile Layer " << i << "\n";
					//			parseTileLayer(*tileLayerNode, levelDetails, *cIter->, tileLayerName);
					//		}
					//	}
					//}
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


