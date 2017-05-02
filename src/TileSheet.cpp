#include "TileSheet.h"
#include "TileSheetDetails.h"

sf::IntRect TileSheet::getTileLocationByID(const TileSheetDetails & details, const int ID)
{
	//Scan through every tile on tilesheet to find appropriate tile by ID
	int col = 0, row = 0;
	const int firstTileID = 0;
	const int endTileID = details.m_rows * details.m_columns;

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
		if (col == details.m_columns)
		{
			col = 0;
			++row;
		}
	}

	//Get location on the tile sheet
	const int tileSize = details.m_tileSize;
	const int margin = details.m_margin;
	const int spacing = details.m_spacing;

	return sf::IntRect((margin + ((spacing + tileSize) * col)),
		(margin + ((spacing + tileSize) * row)),
		tileSize, tileSize);
}

sf::IntRect TileSheet::getTileLocationByPosition(const TileSheetDetails& details, const sf::IntRect & rect)
{
	const int tileSize = details.m_tileSize;
	const sf::IntRect tilePos(rect.left * tileSize, rect.top * tileSize, rect.width * tileSize, rect.height * tileSize);

	return tilePos;
}
