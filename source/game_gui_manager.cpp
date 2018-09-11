#include "render_system.h"
#include <SFML/Graphics/BlendMode.hpp>

void GameGuiManager::setWeaponUpdate(WeaponUpdate& weaponUpdate)
{
	this->weaponUpdate = weaponUpdate;
	if(weaponUpdate.state == WeaponUpdate::WeaponState::SHOOT && weaponUpdate.weaponDef.period >= 0.1)
	{
		shootBlink = 1.0f;
	}
}

void GameGuiManager::setHpUpdate(VariantUpdate& hpUpdate)
{
	this->hpUpdate = hpUpdate;
	if(hpUpdate.get<float>("currentHp") < hpUpdate.get<float>("maxHp"))
	{
		damageBlink = 1.0f;
	}
}

void GameGuiManager::setDeathmatchUpdate(VariantUpdate& deathmatchUpdate)
{
	outputConsole.clear();
	auto actors = deathmatchUpdate.getObjectVector("actors");
	for(int i = 0; i < actors.size(); i++)
	{
		std::string line;
		line += actors[i].get<std::string>("name");
		line += " " + std::to_string(actors[i].get<int>("score"));
		outputConsole.println(line);
	}
}

void GameGuiManager::draw(sf::RenderTarget& renderTarget)
{
	screenWidth = renderTarget.getSize().x;
	screenHeight = renderTarget.getSize().y;
	view.setSize(screenWidth, screenHeight);

	//hp bar
	shape.setTexture(nullptr);
	sf::View targetView = renderTarget.getView();
	renderTarget.setView(view);
	shape.setSize(sf::Vector2f(160 * hpUpdate.get<float>("currentHp") / hpUpdate.get<float>("maxHp"), 32));
	shape.setPosition(screenWidth / 2 - 164, -screenHeight / 2 + 4);
	shape.setFillColor(sf::Color(128, 0, 128, 255));
	renderTarget.draw(shape);

	//hp texture
	shape.setTexture(&getTexture(HP_TEXTURE));
	shape.setSize(sf::Vector2f(32, 32));
	shape.setFillColor(sf::Color::White);
	shape.setPosition(screenWidth / 2 - 200, -screenHeight / 2 + 4);
	renderTarget.draw(shape);

	//weapon texture
	shape.setTexture(&getTexture(weaponUpdate.weaponDef.weaponTexture));
	shape.setPosition(screenWidth / 2 - 200, -screenHeight / 2 + 40);
	renderTarget.draw(shape);

	//reload bar
	shape.setTexture(nullptr);
	float reloadValue = weaponUpdate.weaponData.timeSinceReload / weaponUpdate.weaponDef.reloadTime;
	reloadValue = std::min(reloadValue, 1.0f);
	shape.setSize(sf::Vector2f(160 * reloadValue, 16));
	shape.setPosition(screenWidth / 2 - 164, -screenHeight / 2 + 40);
	shape.setFillColor(sf::Color(0, 255, 255, 128));
	renderTarget.draw(shape);

	//bullets.
	float bulletSize = 160 - (weaponUpdate.weaponDef.bulletsPerHolder - 1) * 4;
	bulletSize /= weaponUpdate.weaponDef.bulletsPerHolder;
	int bullets = weaponUpdate.weaponDef.bulletsPerHolder - weaponUpdate.weaponData.shotsMade;
	shape.setSize(sf::Vector2f(bulletSize, 12));
	for(int i = 0; i < bullets; i++)
	{
		shape.setPosition(screenWidth / 2 - 164 + (bulletSize + 4) * i, -screenHeight / 2 + 60);
		renderTarget.draw(shape);
	}

	//shoot blink
	shape.setSize(sf::Vector2f(screenWidth, screenHeight));
	shape.setPosition(-screenWidth / 2, -screenHeight / 2);
	shape.setTexture(nullptr);
	shape.setFillColor(sf::Color(192, 255, 255, 32.0f * shootBlink));
	renderTarget.draw(shape);

	//damage blink
	shape.setFillColor(sf::Color(255, 0, 0, 128.0f * damageBlink));
	renderTarget.draw(shape);
}

void GameGuiManager::onTimer()
{
	weaponUpdate.weaponData.timeSinceReload += 1.0f / 60.0f;
	weaponUpdate.weaponData.timeSinceShot += 1.0f / 60.0f;
	if(shootBlink > 0)
	{
		shootBlink -= 1.0f / 60.0f / weaponUpdate.weaponDef.period;
	}
	if(weaponUpdate.weaponDef.period < 0.1f)
	{
		shootBlink = 0;
	}
	if(damageBlink > 0)
	{
		damageBlink -= 1.0f / 60.0f;
	}
	damageBlink = std::max(damageBlink, 0.0f);
}


sf::Texture& GameGuiManager::getTexture(std::string name)
{
	if(textures.find(name) == textures.end())
	{
		sf::Texture texture;
		texture.loadFromFile(name);
		textures[name] = texture;
	}
	return textures[name];
}

void GameGuiManager::onKey(int key, bool isReleased)
{
	if(key == sf::Keyboard::Tab)
	{
		outputConsole.setVisible(!isReleased);
	}
}