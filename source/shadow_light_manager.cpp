#include "light_manager.h"


#define LIGHT_VERTEX_SHADER "res/light_vertex_shader.txt"
#define LIGHT_FRAGMENT_SHADER "res/light_fragment_shader.txt"
#define MULTIPLY_FRAGMENT_SHADER "res/divide_fragment_shader.txt"


struct ShadowLightManagerImpl
{
	ShadowLightManagerImpl(float screenWidth, float screenHeight, float tileWidth);
	ShadowLightManagerImpl();
	void init(float screenWidth, float screenHeight, float tileSize);
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

ShadowLightManagerImpl::ShadowLightManagerImpl(float screenWidth, float screenHeight, float tileWidth) :
		shaderArraySize(0),
		shadowsArraySize(0),
		textureDivider(1)
{
	init(screenWidth, screenHeight, tileWidth);
}

ShadowLightManagerImpl::ShadowLightManagerImpl() :
	shaderArraySize(0),
	shadowsArraySize(0),
	textureDivider(1)
{}

void ShadowLightManagerImpl::init(float screenWidth, float screenHeight, float tileSize)
{
	this->screenWidth = screenWidth;
	this->screenHeight = screenHeight;
	this->tileWidth = tileSize;

	sf::FileInputStream vertexStream;
	vertexStream.open(LIGHT_VERTEX_SHADER);
	sf::FileInputStream fragmentStream;
	fragmentStream.open(LIGHT_FRAGMENT_SHADER);
	shader.loadFromStream(vertexStream, fragmentStream);

	sf::FileInputStream multiplyFragmentStream;
	multiplyFragmentStream.open(MULTIPLY_FRAGMENT_SHADER);
	multiplyShader.loadFromStream(multiplyFragmentStream, sf::Shader::Fragment);
	//vertices.setPrimitiveType(sf::Quads);
	renderTexture.create(screenWidth / textureDivider, screenHeight / textureDivider);
	renderTexture.setSmooth(true);

	shape.setSize(sf::Vector2f(screenWidth / tileWidth, screenHeight / tileWidth));
	shape.setOrigin(screenWidth / tileWidth / 2, screenHeight / tileWidth / 2);
}

int ShadowLightManagerImpl::addLightSource(sf::Vector2f pos, sf::Color color, float intensity)
{
	idToShaderIndex[counter] = shaderArraySize;
	shaderIndexToId[shaderArraySize] = counter;
	shader.setUniform("light_pos[" + std::to_string(shaderArraySize) + "]", pos);
	sf::Vector3f colorVec((int) color.r, (int) color.g, (int) color.b);
	colorVec.x /= 255.0f;
	colorVec.y /= 255.0f;
	colorVec.z /= 255.0f;
	shader.setUniform("light_color[" + std::to_string(shaderArraySize) + "]", colorVec);
	shader.setUniform("light_intensity[" + std::to_string(shaderArraySize) + "]", intensity);
	shader.setUniform("sources_size", ++shaderArraySize);

	idToData[counter] = LightData(pos, colorVec, intensity);
	return counter++;
}

int ShadowLightManagerImpl::addRectangleObstacle(sf::Vector2f pos, sf::Vector2f size)
{
	shader.setUniform("shadow_pos[" + std::to_string(shadowsArraySize) + "]", pos);
	shader.setUniform("shadow_size[" + std::to_string(shadowsArraySize) + "]", size);
	shadowsArraySize++;
	shader.setUniform("shadows_size", shadowsArraySize);
	return counter++;
}

void ShadowLightManagerImpl::draw(sf::RenderTarget& renderTarget)
{
	sf::RenderStates renderStates;
	renderStates.shader = &shader;
	renderStates.blendMode = sf::BlendAdd;
	sf::Vector2f center = renderTarget.getView().getCenter();


	double multiplier = tileWidth / textureDivider;
	double offsetX = center.x * multiplier;
	double offsetY = center.y * multiplier;
	//offsetX = offsetX - floor(offsetX);
	//offsetY = offsetY - floor(offsetY);
	offsetX /= multiplier;
	offsetY /= multiplier;

	center.x -= offsetX;
	center.y -= offsetY;

	sf::Vector2f centerInBlocks = center;

	center.x *= tileWidth;
	center.y *= tileWidth;

	shader.setUniform("offset", center);

	std::vector<sf::Vertex> vertices;
	vertices.push_back(sf::Vertex(sf::Vector2f(-screenWidth / 2, -screenHeight / 2)));
	vertices.push_back(sf::Vertex(sf::Vector2f(screenWidth / 2, -screenHeight / 2)));
	vertices.push_back(sf::Vertex(sf::Vector2f(screenWidth / 2, screenHeight / 2)));
	vertices.push_back(sf::Vertex(sf::Vector2f(-screenWidth / 2, screenHeight / 2)));

	sf::View lightView;
	lightView.setCenter(sf::Vector2f(0, 0));
	lightView.setSize(screenWidth, screenHeight);
	renderTexture.setView(lightView);
	renderTexture.clear();
	renderTexture.draw(&vertices[0], vertices.size(), sf::Quads, renderStates);
	renderTexture.display();
	shape.setTexture(&renderTexture.getTexture());
	shape.setPosition(centerInBlocks);

	sf::RenderStates multiplyRenderStates;


	/*
	multiplyRenderStates.blendMode = sf::BlendAdd;
	multiplyShader.setUniform("multiplier", 0.3f);
	multiplyRenderStates.shader = &multiplyShader;
	renderTarget.draw(shape, multiplyRenderStates);
	*/


	multiplyRenderStates.blendMode = sf::BlendMultiply;
	multiplyShader.setUniform("multiplier", 1.0f);
	multiplyRenderStates.shader = &multiplyShader;
	renderTarget.draw(shape, multiplyRenderStates);

}

void ShadowLightManagerImpl::setPosition(int lightSourceID, sf::Vector2f pos)
{
	shader.setUniform("light_pos[" + std::to_string(idToShaderIndex[lightSourceID]) + "]", pos);
}

void ShadowLightManagerImpl::removeLightSource(int lightSourceID)
{
	//should swap with the last element in shader array
	int lastId = shaderIndexToId[shaderArraySize - 1];
	LightData lastData = idToData[lastId];
	int currentIndex = idToShaderIndex[lightSourceID];
	shaderIndexToId[currentIndex] = lastId;
	idToShaderIndex[lastId] = currentIndex;
	shader.setUniform("light_pos[" + std::to_string(currentIndex) + "]", lastData.position);
	shader.setUniform("light_color[" + std::to_string(currentIndex) + "]", lastData.color);
	shader.setUniform("light_intensity[" + std::to_string(currentIndex) + "]", lastData.intensity);
	shaderArraySize--;
	shader.setUniform("sources_size", shaderArraySize);

	//todo: remove lightSourceID data from maps

}

void ShadowLightManagerImpl::onWindowResize(float screenWidth, float screenHeight)
{
	this->screenWidth = screenWidth;
	this->screenHeight = screenHeight;
	shape.setSize(sf::Vector2f(screenWidth / tileWidth, screenHeight / tileWidth));
	shape.setOrigin(screenWidth / tileWidth / 2, screenHeight / tileWidth / 2);
}

ShadowLightManager::ShadowLightManager(float screenWidth, float screenHeight, float tileWidth)
{
	impl = new ShadowLightManagerImpl(screenWidth, screenHeight, tileWidth);
}

ShadowLightManager::ShadowLightManager()
{
	impl = new ShadowLightManagerImpl();
}

ShadowLightManager::~ShadowLightManager()
{
	delete impl;
}

void ShadowLightManager::init(float screenWidth, float screenHeight, float tileSize)
{
	impl->init(screenWidth, screenHeight, tileSize);
}

int ShadowLightManager::addLightSource(sf::Vector2f pos, sf::Color color, float intensity)
{
	return impl->addLightSource(pos, color, intensity);
}

int ShadowLightManager::addRectangleObstacle(sf::Vector2f pos, sf::Vector2f size)
{
	return impl->addRectangleObstacle(pos, size);
}

void ShadowLightManager::draw(sf::RenderTarget& renderTarget)
{
	impl->draw(renderTarget);
}

void ShadowLightManager::setPosition(int lightSourceIndex, sf::Vector2f pos)
{
	impl->setPosition(lightSourceIndex, pos);
}

void ShadowLightManager::removeLightSource(int lightSourceIndex)
{
	impl->removeLightSource(lightSourceIndex);
}

void ShadowLightManager::onWindowResize(float screenWidth, float screenHeight)
{
	impl->onWindowResize(screenWidth, screenHeight);
}