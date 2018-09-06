#ifndef LIGHT_MANAGER_H
#define LIGHT_MANAGER_H

#include<SFML/Graphics.hpp>


struct ILightManager
{
	virtual ~ILightManager() {}
	virtual void init(float screenWidth, float screenHeight, float tileSize) = 0;
	virtual int addLightSource(sf::Vector2f pos, sf::Color color, float intensity, float additiveFactor) = 0;
	virtual int addRectangleObstacle(sf::Vector2f pos, sf::Vector2f size) = 0;
	virtual void draw(sf::RenderTarget& renderTarget) = 0;
	virtual void setPosition(int lightSourceIndex, sf::Vector2f pos) = 0;
	virtual void removeLightSource(int lightSourceIndex) = 0;
	virtual void onWindowResize(float screenWidth, float screenHeight) = 0;
};

struct LightManagerFactory
{
	static ILightManager* createLightManager(float screenWidth, float screenHeight, float tileSize);
};

struct SimpleLightManagerImpl;

struct SimpleLightManager : ILightManager
{
	SimpleLightManager(float screenWidth, float screenHeight, float tileSize);
	SimpleLightManager();
	~SimpleLightManager();
	void init(float screenWidth, float screenHeight, float tileSize);
	int addLightSource(sf::Vector2f pos, sf::Color color, float intensity, float additiveFactor);
	int addRectangleObstacle(sf::Vector2f pos, sf::Vector2f size);
	void draw(sf::RenderTarget& renderTarget);
	void setPosition(int lightSourceIndex, sf::Vector2f pos);
	void removeLightSource(int lightSourceIndex);
	void onWindowResize(float screenWidth, float screenHeight);
private:
	SimpleLightManagerImpl* impl;
};

struct ShadowLightManagerImpl;

struct ShadowLightManager : ILightManager
{
	ShadowLightManager(float screenWidth, float screenHeight, float tileSize);
	ShadowLightManager();
	~ShadowLightManager();
	void init(float screenWidth, float screenHeight, float tileSize);
	int addLightSource(sf::Vector2f pos, sf::Color color, float intensity, float additiveFactor);
	int addRectangleObstacle(sf::Vector2f pos, sf::Vector2f size);
	void draw(sf::RenderTarget& renderTarget);
	void setPosition(int lightSourceIndex, sf::Vector2f pos);
	void removeLightSource(int lightSourceIndex);
	void onWindowResize(float screenWidth, float screenHeight);
private:
	ShadowLightManagerImpl* impl;
};

#endif