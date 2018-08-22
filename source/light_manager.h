#ifndef LIGHT_MANAGER_H
#define LIGHT_MANAGER_H

#include<SFML/Graphics.hpp>


struct ILightManager
{
	virtual ~ILightManager() {}
	virtual int addLightSource(sf::Vector2f pos, sf::Color color, float intensity) = 0;
	virtual int addRectangleObstacle(sf::Vector2f pos, sf::Vector2f size) = 0;
	virtual void draw(sf::RenderTarget& renderTarget) = 0;
	virtual void setPosition(int lightSourceIndex, sf::Vector2f pos) = 0;
	virtual void removeLightSource(int lightSourceIndex) = 0;
	virtual void onWindowResize(float screenWidth, float screenHeight) = 0;
};

struct ShadowLightManagerImpl;

struct ShadowLightManager : ILightManager
{
	ShadowLightManager(float screenWidth, float screenHeight, float tileSize);
	~ShadowLightManager();
	int addLightSource(sf::Vector2f pos, sf::Color color, float intensity);
	int addRectangleObstacle(sf::Vector2f pos, sf::Vector2f size);
	void draw(sf::RenderTarget& renderTarget);
	void setPosition(int lightSourceIndex, sf::Vector2f pos);
	void removeLightSource(int lightSourceIndex);
	void onWindowResize(float screenWidth, float screenHeight);
private:
	ShadowLightManagerImpl* impl;
};

#endif