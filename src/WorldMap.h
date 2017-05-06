#pragma once

#include <unordered_map>
#include <SFML\Graphics.hpp>
#include <string>

class TiXmlElement;
struct LevelDetails;
class WorldMap
{
	struct TileSheetDetails
	{
		TileSheetDetails(std::string&& name, int tileSize, int rows, int columns, double margin, double spacing);

		const std::string m_name;
		const int m_tileSize;
		const int m_rows;
		const int m_columns;
		const int m_margin;
		const int m_spacing;
	};
	
	class TileSheet
	{
	public:
		TileSheet(const TileSheetDetails& tileSheetDetails, const std::string& tileSheetPath);
		~TileSheet();
		TileSheet(const TileSheet& orig);

		const std::string& getFilePath() const;
		const TileSheetDetails& getDetails() const;
		const sf::Texture& getTexture() const;

		sf::IntRect getTileLocationByID(int ID) const;
		sf::IntRect getTileLocationByPosition(const sf::IntRect& rect) const;

	private:
		const TileSheetDetails m_tileSheetDetails;
		const std::string& m_filePath;
		sf::Texture* m_texture;
	};

	class TileLayer
	{
		struct Tile
		{
			Tile(const sf::Vector2i& position, const TileSheet& tileSheet, int tileID);
			Tile(const Tile& orig);

			const sf::Vector2i m_position;
			const int m_tileID;
			const TileSheet& m_tileSheet;
			sf::Sprite m_sprite;
		};

	public:
		TileLayer(const std::vector<std::vector<int>>& tileMapData, const sf::Vector2i& mapSize, const TileSheet& tileSheet, std::string&& name);

		const std::string& getName() const;
		void draw(sf::RenderWindow& window);

	private:
		std::vector<Tile> m_tileMap;
		const std::string m_name;

		void loadInTileMap(const std::vector<std::vector<int>>& tileMapData, const sf::Vector2i& mapSize, const TileSheet& tileSheet);
		void clearMap();
	};

public:
	WorldMap(const std::string& mapName);
	~WorldMap();

	void draw(sf::RenderWindow& window);

private:
	std::vector<WorldMap::TileLayer> m_tileLayers;
	std::unordered_map<std::string, WorldMap::TileSheet> m_tileSheets;

	void assignTileLayer(const std::vector<std::vector<int>>& tileData, const sf::Vector2i& mapSize, const std::string& tileSheetName, std::string&& tileLayerName);
	bool hasTileLayer(const std::string& tileLayerName) const;
	bool hasTileSheet(const std::string& tileSheetName) const;
	void parseLevel(const std::string& mapName);
	void clearMap();
	void addTileSheet(const std::string& tileSheetPath, const TileSheetDetails& tileSheetDetails);

	void parseTileMap(const TiXmlElement & root, const LevelDetails & levelDetails);
	void parseTileSheets(const TiXmlElement& root);
	std::vector<std::vector<int>> decodeTileLayer(const TiXmlElement & tileLayerElement, const LevelDetails & levelDetails) const;
	int getNumberOfTileLayers(const TiXmlElement& root) const;
	int getNumberOfTileSets(const TiXmlElement& root) const;

	const TileSheet& getTileSheet(const std::string& name) const;
	const TiXmlElement* findNode(const TiXmlElement& root, const std::string& name) const;
	const TiXmlElement* findNode(const TiXmlElement& root, const std::string& value, const std::string& name) const;
};