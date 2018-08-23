#include <SFML/System.hpp>
#include <SFML/Window.hpp>
#include <SFML/Graphics.hpp>
#include <SFML/Audio.hpp>
#include <SFML/Network.hpp>

#include <Box2D/Box2D.h>

#include "light_manager.h"

void testBox2d()
{
	b2Vec2 gravity(0.0f, -10.0f);
	b2World world(gravity);
	b2BodyDef groundBodyDef;
	groundBodyDef.position.Set(0.0f, -10.0f);
	b2Body* groundBody = world.CreateBody(&groundBodyDef);
}

int main(int argc, char *argv[])
{
	testBox2d();

	sf::VideoMode screen(sf::VideoMode::getDesktopMode());
	sf::RenderWindow window(screen, "");
	window.setFramerateLimit(30);
	sf::View view(sf::Vector2f(0, 0), sf::Vector2f(screen.width / 32, screen.height / 32));
	window.setView(view);
	sf::Color background = sf::Color::White;

	ShadowLightManager lightManager(screen.width, screen.height, 32);
	SimpleLightManager simpleLightManager(screen.width, screen.height, 32);
	for (int i = 0; i < 100; i++)
	{
		simpleLightManager.addLightSource(sf::Vector2f(rand() % 1000 - 500, rand() % 1000 - 500), sf::Color(rand() % 64, rand() % 64, rand() % 64), rand() % 20 + 5);
	}
	simpleLightManager.addLightSource(sf::Vector2f(0, 0), sf::Color::Magenta, 10);
	simpleLightManager.addLightSource(sf::Vector2f(-100, 0), sf::Color::Magenta, 10);
	//lightManager.addRectangleObstacle(sf::Vector2f(0, 0), sf::Vector2f(32, 32));

	sf::RectangleShape rect(sf::Vector2f(1, 1));
	rect.setOrigin(0.5, 0.5);

	while (window.isOpen())
	{
		sf::Event event;
		while (window.pollEvent(event))
		{
			switch (event.type)
			{
				case sf::Event::Closed:
					window.close();
					break;
				case sf::Event::KeyPressed:
					if (event.key.code == sf::Keyboard::Escape)
					{
						window.close();
					}
					break;
				case sf::Event::Resized:
					/*
					view.setSize(event.size.width / 32, event.size.height / 32);
					view.setCenter(event.size.width / 64, event.size.height / 64);
					lightManager.onWindowResize(event.size.width, event.size.height);
					window.setView(view);
					 */
					break;
				case sf::Event::TouchBegan:
				   	//todo: move light source
					break;
			}
		}

		window.clear(background);
		window.draw(rect);
		simpleLightManager.draw(window);
		window.display();

	}
}
