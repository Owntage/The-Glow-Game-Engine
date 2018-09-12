#include <SFML/System.hpp>
#include <SFML/Window.hpp>
#include <SFML/Graphics.hpp>
#include <SFML/Audio.hpp>
#include <SFML/Network.hpp>
#include <Box2D/Box2D.h>
#include <vector>

#include "game_logic.h"
#include "render_system.h"
#include "gui.h"

using namespace std;

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
	sf::Color background = sf::Color::Black;

	GuiManager guiManager(screen.width, screen.height);
	Console console(0, 0, 200, 200, guiManager);
	console.setVisible(false);

	ActorFactory actorFactory("res/properties.xml");
	GameLogic gameLogic(actorFactory);
	int mainActor = gameLogic.createActor("testActor");

	gameLogic.createActor("cyan_light");



	RenderSystem renderSystem(console, guiManager, screen.width, screen.height);
	renderSystem.setMainActor(mainActor);
	renderSystem.onUpdate(gameLogic.getUpdates(0));

	while (window.isOpen())
	{
		sf::Event event;
		while (window.pollEvent(event))
		{
			guiManager.onEvent(event);
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
					//todo: somehow handle window resize
					break;
			}
		}

		Event timeEvent("timer");
		gameLogic.onEvent(timeEvent);

		renderSystem.onUpdate(gameLogic.getUpdates(0));

		window.clear(background);
		renderSystem.draw(window);
		guiManager.draw(window);
		window.display();

	}
}
