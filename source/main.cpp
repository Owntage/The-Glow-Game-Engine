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
#include "components/move_event.h"
#include "components/world.h"
#include "performance_profiler.h"

using namespace std;



int main(int argc, char *argv[])
{
	sf::VideoMode screen(sf::VideoMode::getDesktopMode());
	sf::RenderWindow window(screen, "");
	window.setFramerateLimit(30);
	sf::View view(sf::Vector2f(0, 0), sf::Vector2f(screen.width / 64, screen.height / 64));
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

	int mainActor = gameLogic.createActor("testActor");
	CoordEvent coordEvent("set_coords", mainActor, 20, 30);
	gameLogic.onEvent(coordEvent);

	gameLogic.createActor("cyan_light");

	PerformanceProfiler performance;
	RenderSystem renderSystem(performance, console, guiManager, screen.width, screen.height);
	renderSystem.setMainActor(mainActor);

	NinePatchSprite buttonSprite("res/background.png", true);
	NinePatchSprite hoveredSprite("res/gui/console/slider.png", true);

	Button upButton(0, -200, 100, 100);
	upButton.setNormalSprite(buttonSprite);
	upButton.setHoveredSprite(hoveredSprite);
	upButton.setPressedSprite(buttonSprite);

	Button downButton(0, 200, 100, 100);
	downButton.setNormalSprite(buttonSprite);
	downButton.setHoveredSprite(hoveredSprite);
	downButton.setPressedSprite(buttonSprite);

	Button leftButton(-200, 0, 100, 100);
	leftButton.setNormalSprite(buttonSprite);
	leftButton.setHoveredSprite(hoveredSprite);
	leftButton.setPressedSprite(buttonSprite);

	Button rightButton(200, 0, 100, 100);
	rightButton.setNormalSprite(buttonSprite);
	rightButton.setHoveredSprite(hoveredSprite);
	rightButton.setPressedSprite(buttonSprite);

	guiManager.addElement(upButton);
	guiManager.addElement(downButton);
	guiManager.addElement(leftButton);
	guiManager.addElement(rightButton);

	bool up = false;
	bool down = false;
	bool left = false;
	bool right = false;

	upButton.setOnPressCallback([&up](){ up = true; });
	downButton.setOnPressCallback([&down](){ down = true; });
	leftButton.setOnPressCallback([&left](){ left = true; });
	rightButton.setOnPressCallback([&right](){ right = true; });

	upButton.setOnReleaseCallback([&up](){ up = false; });
	downButton.setOnReleaseCallback([&down](){ down = false; });
	leftButton.setOnReleaseCallback([&left](){ left = false; });
	rightButton.setOnReleaseCallback([&right](){ right = false; });

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
					if (event.key.code == sf::Keyboard::Delete)
					{
						std::cout << "render: " << performance.getSectionRelativeTime("render") << std::endl;
						std::cout << "tiles: " << performance.getSectionRelativeTime("tiles") << std::endl;
						std::cout << "game ui: " << performance.getSectionRelativeTime("game_gui") << std::endl;
						std::cout << "light: " << performance.getSectionRelativeTime("light") << std::endl;
						std::cout << "tiles preparation: " << performance.getSectionRelativeTime("tiles_prepare") << std::endl;
						std::cout << "actors: " << performance.getSectionRelativeTime("actors") << std::endl;
					}
					break;
				case sf::Event::Resized:
					//todo: somehow handle window resize
					break;
			}
		}

		performance.enterSection("logic");
		gameLogic.onEvent(timer);
		World::getInstance()->update(60);

		MoveEvent moveEvent(left, right, up, down, 0, mainActor);
		gameLogic.onEvent(moveEvent);

		renderSystem.onUpdate(gameLogic.getUpdates(systemIndex));
		performance.exitSection("logic");


		window.clear(background);
		performance.enterSection("render");
		renderSystem.draw(window);
		guiManager.draw(window);
		performance.exitSection("render");
		window.display();


	}
}
