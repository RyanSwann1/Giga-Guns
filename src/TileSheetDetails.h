#pragma once

struct TileSheetDetails
{
	TileSheetDetails(int tileSize, int rows, int columns, int margin, int spacing)
		: m_tileSize(tileSize),
		m_rows(rows),
		m_columns(columns),
		m_margin(margin),
		m_spacing(spacing)
	{}

	const int m_tileSize;
	const int m_rows;
	const int m_columns;
	const int m_margin;
	const int m_spacing;
};