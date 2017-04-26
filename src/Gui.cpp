#include <assert.h>

#include "Gui.h"


sf::Uint32 getId(const sf::String & text)
{
	/* Hash (djb2 by dan bernstein) */
	int hash = 5381;
	const sf::Uint32 *data = text.getData();
	sf::Uint32 codepoint;
	while ((codepoint = *data++))
		hash = ((hash << 5) + hash) + codepoint;
	return hash;
}

Gui::Gui(sf::Font& font, sf::RenderWindow& window) :
	m_font(font),
	m_window(window),
	m_hotId(0),
	m_activeId(0)
{
}

bool Gui::Button(const sf::String& label, sf::Vector2f position)
{
	sf::RectangleShape rect{ sf::Vector2f{ 100, 40 } };
	sf::Text text{ label, m_font };
	sf::Uint32 id = getId(label);
	assert(id != 0);

	rect.setPosition(position);
	text.setPosition(position);

	bool containsMouse = rect.getGlobalBounds().contains(sf::Vector2f{ sf::Mouse::getPosition(m_window) });
	bool pressed = false;

	if (m_hotId == id)
	{
		if (!containsMouse)
		{
			m_hotId = 0;
		}
		else if (sf::Mouse::isButtonPressed(sf::Mouse::Left))
		{
			m_hotId = 0;
			m_activeId = id;
		}
	}
	else if (m_activeId == id)
	{
		if (!sf::Mouse::isButtonPressed(sf::Mouse::Left))
		{
			if (containsMouse)
				pressed = true;
			m_activeId = 0;
		}
	}
	else if (!sf::Mouse::isButtonPressed(sf::Mouse::Left) && containsMouse)
	{
		m_hotId = id;
	}

	rect.setOutlineThickness((m_hotId == id || m_activeId == id) ? 4.f : 2.f);
	rect.setOutlineColor(sf::Color::Black);
	m_window.draw(rect);
	text.setFillColor(sf::Color::Black);
	m_window.draw(text);
	return pressed;
}