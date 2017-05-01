#pragma once
#include <SFML\Graphics.hpp>

struct TileSheetDetails;

namespace TileSheet
{
	//inline const TileSheetDetails& getDetails() { return m_details; }
	sf::IntRect getTileLocationByID(const TileSheetDetails& details, const int ID);
	sf::IntRect getTileLocationByPosition(const TileSheetDetails& details, const sf::IntRect& rect);
}