#pragma once

#include <unordered_map>
#include <SFML\Graphics.hpp>
#include <string>
#include <memory>

class TiXmlElement;
class WorldMap
{
	class LevelDetails
	{
	public:
		LevelDetails()
			: m_tileSize(0),
			m_mapSize()
		{

		}
		LevelDetails(int tileSize, const sf::Vector2i& mapSize)
			: m_tileSize(tileSize),
			m_mapSize(mapSize)
		{}

		int m_tileSize;
		sf::Vector2i m_mapSize;
	};

	class TileSheetDetails
	{
	public:
		TileSheetDetails(const std::string& name, int tileSize, int rows, int columns, int firstGID, double margin, double spacing);

		const std::string m_name;
		const int m_tileSize;
		const int m_rows;
		const int m_columns;
		const int m_firstGID;
		const double m_margin;
		const double m_spacing;
	};
	
	class TileSheet
	{
	public:
		TileSheet(const TileSheetDetails& tileSheetDetails, const std::string& tileSheetPath);
		TileSheet(TileSheet&& orig);

		const std::string& getFilePath() const;
		const TileSheetDetails& getDetails() const;
		const sf::Texture& getTexture() const;

		sf::IntRect getTileLocationByID(int ID) const;
		sf::IntRect getTileLocationByPosition(const sf::IntRect& rect) const;

	private:
		const TileSheetDetails m_tileSheetDetails;
		const std::string m_filePath;
		std::unique_ptr<sf::Texture> m_texture;
	};

	class TileLayer
	{
		class Tile
		{
		public:
			Tile(const sf::Vector2f& position, const TileSheet& tileSheet, int tileID);
			Tile(const Tile& orig);

			const sf::FloatRect m_position;
			const int m_tileID;
			const TileSheet& m_tileSheet;
			sf::Sprite m_sprite;
		};

	public:
		TileLayer(const std::vector<std::vector<int>>& tileMapData, const sf::Vector2i& mapSize, const TileSheet& tileSheet);
		void draw(sf::RenderWindow& window);

	private:
		std::vector<Tile> m_tileMap;
	};

	class CollidableTileLayer
	{
	public:
		void addCollidableTile(const sf::Vector2i& tilePosition, int tileSize);
		const std::vector<sf::Vector2i>& getTileMap() const;

	private:
		std::vector<sf::Vector2i> m_tileMap;
	};

public:
	WorldMap(const std::string& mapName);
	 
	const CollidableTileLayer& getCollidableTileLayer() const;
	const LevelDetails& getLevelDetails() const;
	void draw(sf::RenderWindow& window);

private:
	std::vector<TileLayer> m_tileLayers;
	std::unordered_map<std::string, TileSheet> m_tileSheets;
	CollidableTileLayer m_collidableTileLayer;
	LevelDetails m_levelDetails;

	void parseTileMap(const TiXmlElement & root);
	void parseCollidableLayer(const TiXmlElement& root);
	void parseTileSheets(const TiXmlElement& root);
	std::vector<std::vector<int>> decodeTileLayer(const TiXmlElement & tileLayerElement, const std::string& tileSheetName) const;
	const TileSheet& getTileSheet(const std::string& name) const;
	LevelDetails parseLevelDetails(const TiXmlElement& root) const;
};