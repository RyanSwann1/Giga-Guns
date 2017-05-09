#pragma once

#include <unordered_map>
#include <SFML\Graphics.hpp>
#include <string>
#include <memory>

class TiXmlElement;
class LevelDetails;
class WorldMap
{
	class TileSheetDetails
	{
	public:
		TileSheetDetails(const std::string& name, int tileSize, int rows, int columns, double margin, double spacing);

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
			Tile(const sf::Vector2i& position, const TileSheet& tileSheet, int tileID);
			Tile(const Tile& orig);

			const sf::Vector2i m_position;
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

public:
	WorldMap(const std::string& mapName);

	void draw(sf::RenderWindow& window);

private:
	std::vector<WorldMap::TileLayer> m_tileLayers;
	std::unordered_map<std::string, WorldMap::TileSheet> m_tileSheets;

	void parseTileMap(const TiXmlElement & root, const LevelDetails & levelDetails);
	void parseTileSheets(const TiXmlElement& root);

	std::vector<std::vector<int>> decodeTileLayer(const TiXmlElement & tileLayerElement, const LevelDetails & levelDetails) const;
	bool hasTileSheet(const std::string& tileSheetName) const;
	const TileSheet& getTileSheet(const std::string& name) const;
};