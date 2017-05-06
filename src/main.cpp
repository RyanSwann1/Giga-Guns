#include <iostream>

#include "Gui.h"
#include "WorldMap.h"
#include <fstream>
#include <SFML/Graphics.hpp>

int main()
{
	sf::RenderWindow window(sf::VideoMode(750, 750), "Giga-Guns!");
	WorldMap worldMap("Map.tmx");

	// load font
	sf::Font font;
	if (!font.loadFromFile("Roboto.ttf"))
	{
		std::cout << "Failed to load Roboto.ttf" << std::endl;
		return 1;
	}

	Gui gui{ font, window };
	bool inMenu = true;

	while (window.isOpen())
	{
		sf::Event event;
		while (window.pollEvent(event))
		{
			if (event.type == sf::Event::Closed)
			{
				window.close();
			}
		}

		window.clear(sf::Color::White);
		worldMap.draw(window);

		if (inMenu)
		{
			// main menu
			if (gui.Button("Play", sf::Vector2f{ 50, 50 }))
			{
				inMenu = false;
			}
			if (gui.Button("Quit", sf::Vector2f{ 50, 110 }))
			{
				window.close();
			}
		}
		else
		{
			// "level"
			if (gui.Button("Menu", sf::Vector2f{ 50, 80 }))
			{
				inMenu = true;
			}
		}
		window.display();
	}

	return 0;
}