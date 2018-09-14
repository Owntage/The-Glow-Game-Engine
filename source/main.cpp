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
#include "level_loader.h"
#include "components/coord_event.h"

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
	Console console(0, 0, 400, 200, guiManager);
	console.setVisible(false);

	ActorFactory actorFactory("res/properties.xml");
	GameLogic gameLogic(actorFactory);
	LevelLoader levelLoader(gameLogic);
	levelLoader.loadLevel("res", "level1.tmx");

	Event timer("timer");
	gameLogic.onEvent(timer);
	gameLogic.onEvent(timer);
	gameLogic.onEvent(timer);
	gameLogic.onEvent(timer);
	gameLogic.onEvent(timer);

	int mainActor = gameLogic.createActor("testActor");
	CoordEvent coordEvent("set_coords", mainActor, 20, 30);
	gameLogic.onEvent(coordEvent);

	gameLogic.createActor("cyan_light");

	RenderSystem renderSystem(console, guiManager, screen.width, screen.height);
	renderSystem.setMainActor(mainActor);

	int systemIndex = gameLogic.registerSystem();

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
						return 0;
					}
					break;
				case sf::Event::Resized:
					//todo: somehow handle window resize
					break;
			}
		}

		Event timer("timer");
		//gameLogic.onEvent(timer);
		renderSystem.onUpdate(gameLogic.getUpdates(systemIndex));

		window.clear(background);
		renderSystem.draw(window);
		guiManager.draw(window);
		window.display();

	}
}
