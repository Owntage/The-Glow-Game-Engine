#ifndef RENDER_SYSTEM_H
#define RENDER_SYSTEM_H


#include <vector>
#include <set>
#include <memory>
#include <map>
#include <SFML/Graphics.hpp>
#include "gui.h"
#include "light_manager.h"
#include "actor_update.h"
#include "components/animation_component.h"
#include "components/render_component.h"
#include "components/weapon_component.h"
#include "components/hp_component.h"
#include "components/deathmatch_component.h"
#include "components/variant_update.h"
#include "sound_manager.h"


#define MULTIPLY_FRAGMENT_SHADER "res/divide_fragment_shader.txt"

#define HP_TEXTURE "res/gui/hp.png"
#define NO_WEAPON_TEXTURE "res/gui/no_weapon.png"

struct GameGuiManager
{
	GameGuiManager(float screenWidth, float screenHeight, GuiManager& guiManager) :
		screenWidth(screenWidth),
		screenHeight(screenHeight),
		view(sf::Vector2f(0, 0), sf::Vector2f(screenWidth, screenHeight)),
		shootBlink(0.0f),
		damageBlink(0.0f),
		outputConsole(0, -100, 200, 300, guiManager)
	{
		hpUpdate.set("maxHp", 1.0f);
		hpUpdate.set("currentHp", 1.0f);

		outputConsole.setVisible(false);
	}
	void setWeaponUpdate(WeaponUpdate& weaponUpdate);
	void setHpUpdate(VariantUpdate& hpUpdate);
	void setDeathmatchUpdate(VariantUpdate& deathmatchUpdate);
	void draw(sf::RenderTarget& renderTarget);
	void onTimer();
	void onKey(int key, bool isReleased);

private:
	float screenWidth;
	float screenHeight;
	float shootBlink;
	float damageBlink;
	sf::View view;
	sf::RectangleShape shape;
	std::map<std::string, sf::Texture> textures;
	WeaponUpdate weaponUpdate;
	VariantUpdate hpUpdate;
	sf::Texture& getTexture(std::string name);
	OutputConsole outputConsole;
};



#define DEFAULT_TEXTURE "res/graphic/default.png"

struct RenderSystem;

struct DrawableActor
{
	DrawableActor(Console& console, RenderSystem& renderSystem, SoundManager& soundManager, std::shared_ptr<ILightManager> lightManager);
	~DrawableActor();
	void onUpdate(ActorUpdate& actorUpdate);
	void draw(sf::RenderTarget& renderTarget);
	void setMain(bool isMain);
private:
	bool isMain;
	bool isDrawing;
	bool animationStateChanged;
	float serverTime;

	float deltaTime;
	float positionX;
	float positionY;
	float speedX;
	float speedY;
	float sizeX;
	float sizeY;

	std::vector<LayerState> animationLayerStates;
	std::vector<float> layerTime;
	std::vector<int> layerImageIndex;
	std::map<std::string, AnimationState> animationStates;

	sf::RectangleShape rect;
	sf::Vertex vertices[4];
	
	Console& console;
	int lastMessagePrinted;
	RenderSystem& renderSystem;
	SoundManager& soundManager;
	
	std::shared_ptr<ILightManager> lightManager;
	bool hasLightSource;
	bool hasShadow;
	int lightSourceID;

	RenderData renderData;

};

#define TILE_SIZE 64
#define TILESET_WIDTH 16
#define TILESET_HEIGHT 16



struct RenderSystem
{
	RenderSystem(Console& console, GuiManager& guiManager, float screenWidth, float screenHeight);
	void onUpdate(std::vector<std::shared_ptr<ActorUpdate> > updates);
	void draw(sf::RenderTarget& renderTarget);
	void setMainActor(int mainActor);
	void onImageLoaded(std::string image);
	float getCameraX();
	float getCameraY();
	float getPlayerX();
	float getPlayerY();
	void onKey(int key, bool isReleased);
	std::vector<std::string> imagesToLoad;
private:
	std::map<int, std::shared_ptr<DrawableActor> > actors;
	std::map<std::string, sf::Texture> textures;
	std::map<std::string, int> imagesInTileset;
	std::set<int> deletedActors;
	std::set<std::string> imageLoadRequests;
	int mainActor;
	float playerX;
	float playerY;
	float cameraX;
	float cameraY;
	sf::Vector2f newGameViewPos;
	sf::View gameView;
	sf::VertexArray tileVertices;
	sf::Texture tileset;
	Console& console;
	SoundManager soundManager;
	std::shared_ptr<ILightManager> lightManager;
	GameGuiManager gameGuiManager;
	friend class DrawableActor;
};

#endif