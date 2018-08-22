#ifndef LIGHT_MANAGER_H
#define LIGHT_MANAGER_H

#include <SFML/System.hpp>
#include <SFML/Graphics.hpp>
#include <SFML/Window.hpp>

#define LIGHT_VERTEX_SHADER "res/light_vertex_shader.txt"
#define LIGHT_FRAGMENT_SHADER "res/light_fragment_shader.txt"
#define MULTIPLY_FRAGMENT_SHADER "res/divide_fragment_shader.txt"

struct LightManager
{
	LightManager(float screenWidth, float screenHeight, float tileWidth);
	int addLightSource(sf::Vector2f pos, sf::Color color, float intensity);
	int addRectangleObstacle(sf::Vector2f pos, sf::Vector2f size);
	void draw(sf::RenderTarget& renderTarget);
	void setPosition(int lightSourceIndex, sf::Vector2f pos);
	void removeLightSource(int lightSourceIndex);
	void onWindowResize(float screenWidth, float screenHeight);
private:
	struct LightData
	{
		sf::Vector2f position;
		sf::Vector3f color;
		float intensity;
		LightData() {}
		LightData(sf::Vector2f position, sf::Vector3f color, float intensity) :
				position(position),
				color(color),
				intensity(intensity)
		{}
	};
	std::map<int, int> idToShaderIndex; //can be replaced with bimap
	std::map<int, int> shaderIndexToId;
	std::map<int, LightData> idToData;
	int shaderArraySize;
	int shadowsArraySize;
	int counter;
	int textureDivider;
	sf::Shader shader;
	sf::Shader multiplyShader;
	sf::RectangleShape shape;
	sf::RenderTexture renderTexture;
	float screenWidth;
	float screenHeight;
	float tileWidth;
};

#endif