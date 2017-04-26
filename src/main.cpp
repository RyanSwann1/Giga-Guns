#include <iostream>

#include <SFML/Graphics.hpp>

#include "Gui.h"

int main()
{
	sf::RenderWindow window(sf::VideoMode(200, 200), "Giga-Guns!");

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