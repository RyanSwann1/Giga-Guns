#include "Gui.h"
#include "WorldMap.h"
#include "CollisionHandler.h"
#include <fstream>
#include <SFML/Graphics.hpp>
#include <iostream>

#define PLAYER_SPEED 100

int main()
{
	sf::RenderWindow window(sf::VideoMode(800, 800), "Giga-Guns!");
	WorldMap worldMap("Map.tmx");

	// load font
	sf::Font font;
	if (!font.loadFromFile("Roboto.ttf"))
	{
		std::cout << "Failed to load Roboto.ttf" << std::endl;
		return 1;
	}

	sf::RectangleShape player{ sf::Vector2f{ 20, 20 } };
	player.setPosition(200, 200);
	player.setFillColor(sf::Color::Black);
	Gui gui{ font, window };
	bool inMenu = true, levelMenuOpen = false, escapeKeyPressed;
	sf::Clock frameClock;

	while (window.isOpen())
	{
		sf::Time lastFrameTime = frameClock.restart();
		escapeKeyPressed = false;

		sf::Event event;
		while (window.pollEvent(event))
		{
			if (event.type == sf::Event::Closed)
			{
				window.close();
			}
			else if (event.type == sf::Event::KeyPressed && event.key.code == sf::Keyboard::Escape)
			{
				escapeKeyPressed = true;
			}
		}

		window.clear(sf::Color::White);
		worldMap.draw(window);

		if (inMenu)
		{
			// main menu
			if (gui.Button("Play", sf::Vector2f{ 150, 150 }))
			{
				inMenu = false;
			}
			if (gui.Button("Quit", sf::Vector2f{ 150, 210 }))
			{
				window.close();
			}
		}
		else if (levelMenuOpen)
		{
			// "level" menu
			if (gui.Button("Menu", sf::Vector2f{ 50, 80 }))
			{
				inMenu = true;
				levelMenuOpen = false;
			}
			if (escapeKeyPressed)
			{
				levelMenuOpen = false;
			}
		}
		else
		{
			sf::Vector2f movement;
			if (sf::Keyboard::isKeyPressed(sf::Keyboard::Left))
			{
				movement.x -= PLAYER_SPEED * lastFrameTime.asSeconds();
			}
			else if (sf::Keyboard::isKeyPressed(sf::Keyboard::Right))
			{
				movement.x += PLAYER_SPEED * lastFrameTime.asSeconds();
			}
			else if (sf::Keyboard::isKeyPressed(sf::Keyboard::Down))
			{
				movement.y += PLAYER_SPEED * lastFrameTime.asSeconds();
			}
			else if (sf::Keyboard::isKeyPressed(sf::Keyboard::Up))
			{
				movement.y -= PLAYER_SPEED * lastFrameTime.asSeconds();
			}
			if (escapeKeyPressed)
			{
				levelMenuOpen = true;
			}

			const sf::Vector2f playerPosition = player.getPosition();
			CollisionHandler::clampMovement(movement, playerPosition, player.getSize(), worldMap);
			player.setPosition(playerPosition + movement);
			window.draw(player);
		}
		window.display();
	}
	return 0;
}
