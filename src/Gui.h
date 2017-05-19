#include <SFML/Graphics.hpp>

class Gui
{
public:
	Gui(sf::Font& font, sf::RenderWindow& window);

	bool Button(const sf::String& label, sf::Vector2f position);

private:
	sf::Font& m_font;
	sf::RenderWindow& m_window;
	sf::Uint32 m_hotId;
	sf::Uint32 m_activeId;
};