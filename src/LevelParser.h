#pragma once

class WorldMap;
#include <string>

namespace LevelParser
{
	void parseLevel(WorldMap& worldMap, const std::string& levelName);
}
